package main

/* Decided to use Redis because it's so much more veratile than memcached.
 * Normally we would also use distributed locks (github.com/bsm/redislock), but
 * this example is very simple so they're not terribly important here.
 *
 * We can use Redis as a cache and a database with little difficulty.  Isn't
 * that convenient?
 *
 * Get Redis bindings: `go get github.com/go-redis/redis/v8`
 * All other libraries are built in. */

import (
    "context"
    "errors"
    "flag"
    "fmt"
    "html"
    "io"
    "os"
    "sort"
    "strings"
    "time"
    "io/ioutil"
    "net/http"
    "os/signal"
    "sync/atomic"
    "github.com/go-redis/redis/v8"
)

type handler struct {
    mux map[string]func(
        context.Context,
        redis.Client,
        http.ResponseWriter,
        *http.Request,
    ) error
    ctx context.Context
    rdb redis.Client
}

var toSave uint32 = 0

// originally intended to store numerically in SQL; didn't feel like rewriting
// all the code when I noticed that Redis could be my database
// (I also forgot that SQL has a datetime type)
const timeFormat string = "20060102150405.000"

func parseComment(key string, value string) (time.Time, string, string, error) {
    comment := html.EscapeString(value)
    k := strings.SplitN(html.EscapeString(key), " ", 2)
    if len(k) != 2 {
        return time.Time{}, "", comment, errors.New("Bad key format: " + key)
    }
    t, err := time.Parse(timeFormat, k[0])
    if err != nil {
        return time.Time{}, k[1], comment, errors.New("Bad timestamp: " + k[0])
    }
    return t, k[1], comment, nil
}

func homepage(
    ctx context.Context,
    rdb redis.Client,
    writer http.ResponseWriter,
    req *http.Request,
) error {
    name := req.PostFormValue("name")
    comment := req.PostFormValue("comment")

    val, err := rdb.Get(ctx, "home").Result()

    if err == redis.Nil {
        // not cached
        v, err := ioutil.ReadFile("home.html")
        val = string(v)
        if err != nil {
            // can't serve a page we can't read
            return err
        } else {
            // default expiration time is probably fine
            err := rdb.SetNX(ctx, "home", val, 5 * time.Minute).Err()
            if err != nil {
                // couldn't cache, but can still serve page
                fmt.Fprintf(os.Stdout, "%v\n", err)
            } else {
                fmt.Println("Cached homepage.")
            }
        }
    } else if err == nil {
        // cached
        fmt.Println("Retrieved homepage from cache.")
    } else {
        // some unknown error
        return err
    }

    // save comment
    if name != "" && comment != "" {
        t := time.Now().UTC().Format(timeFormat)
        key := t + " " + name
        rdb.HSet(ctx, "comment", key, comment)
        atomic.AddUint32(&toSave, 1)
    } else if name != "" || comment != "" {
        // empty name or comment, but not both
        val = strings.Replace(
            val, "id=\"err\" style=\"display:none\">",
            "id=\"err\"><p>Name or comment is missing.</p>", 1,
        )
    }

    // load comments
    comments, err := rdb.HGetAll(ctx, "comment").Result()
    if err != nil {
        fmt.Fprintf(os.Stderr, "%v\n", err)
    }
    entries := make([]string, 0, len(comments))
    for k := range comments {
        timestamp, name, comment, err := parseComment(k, comments[k])
        if err != nil {
            fmt.Fprintf(os.Stderr, "%v\n", err)
        } else {
            str := fmt.Sprintf(
                "    <tr><td>%s</td><td>%s</td><td>%s</td></tr>",
                timestamp.Format("2006-01-02 15:04:05.000"),
                name,
                strings.Replace(comment, "\n", "<br/>", -1),
            )
            entries = append(entries, str)
        }
    }
    sort.Sort(sort.Reverse(sort.StringSlice(entries)))
    val = strings.Replace(
        val, "<tbody>", "<tbody>\n" + strings.Join(entries, "\n"), 1,
    )

    io.WriteString(writer, val)
    fmt.Println("Served homepage.")
    return nil
}

func (hand *handler) ServeHTTP(writer http.ResponseWriter, req *http.Request) {
    if h, ok := hand.mux[req.URL.String()]; ok {
        err := h(hand.ctx, hand.rdb, writer, req)
        if err != nil {
            fmt.Fprintf(os.Stderr, "%v\n", err)
        }
    } else {
        http.Error(writer, http.StatusText(404), 404)
    }
}

func save(
    ctx context.Context, rdb redis.Client,
    minDelay time.Duration, maxDelay time.Duration,
    increaseThreshold uint32, decreaseThreshold uint32,
) {
    delay := minDelay
    for {
        time.Sleep(delay)
        nToSave := atomic.SwapUint32(&toSave, 0)
        if (nToSave > 0) {
            fmt.Printf("%d new comment(s) since last save.\n", nToSave)
            status := rdb.BgSave(ctx)
            fmt.Println(status)
        }
        // naive adjustment assumes write rate changes smoothly
        if (nToSave >= decreaseThreshold && delay > minDelay) {
            delay /= 2
        } else if (nToSave <= increaseThreshold && delay < maxDelay) {
            delay *= 2
        }
    }
}

func shutdown(ctx context.Context, rdb redis.Client) {
    fmt.Println(rdb.Save(ctx))
    rdb.Close()
}

func main() {
    host := flag.String("h", "localhost:6379", "Redis host")
    pwd := flag.String("p", "", "Redis password")
    db := flag.Int("d", 0, "database number")
    ctx := context.Background()

    // setup Redis database client (requires running server)
    rdb := redis.NewClient(&redis.Options {
        Addr:     *host,
        Password: *pwd,
        DB:       *db,
    })
    // Handle interrupt gracefully
    go func() {
        ch := make(chan os.Signal)
        signal.Notify(ch, os.Interrupt)
        <-ch
        fmt.Println("\nInterrupted")
        shutdown(ctx, *rdb)
        os.Exit(0)
    }()
    // Close if main function exits instead of interrupt
    defer shutdown(ctx, *rdb)

    // try to ping Redis
    result, err := rdb.Ping(ctx).Result()
    fmt.Println(result, err)
    if err != nil {
        fmt.Fprintf(os.Stderr, "Is Redis running and configured correctly?\n")
        os.Exit(1)
    }

    // generic multiplexing for extensibility
    mux := make(map[string]func(
        context.Context,
        redis.Client,
        http.ResponseWriter,
        *http.Request,
    ) error)
    mux["/"] = homepage

    // setup HTTP server
    server := http.Server {
        Addr:    ":8000",
        Handler: &handler {
            mux: mux,
            ctx: ctx,
            rdb: *rdb,
        },
    }

    // start save and listen routines
    go save(ctx, *rdb, 675 * time.Millisecond, 10 * time.Second, 0, 128)
    server.ListenAndServe()
}
