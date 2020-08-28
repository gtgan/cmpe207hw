// ----------------------------------- hw1.c -----------------------------------
//
// Should work for IPv4 and IPv6 on Unix, and IPv4 on Windows.
//
// Pass the command line argument -4 for IPv4 (default), -6 for IPv6, or an IP
// address, in which case we try to guess the IP version.  Output goes to stdout
// and there's some logging to stderr, but don't count on the latter to be
// useful.
//
// Below is the sort of mess that I get when I start at the top and just type
// for well over a hundred lines, slap in a few comments, and call it a day.
// Enter if you dare.
//
// – Gregory Gan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// Include platform-specific libraries.
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

const char *const INIT_4 = "94.142.241.111";
const char *const INIT_6 = "2a02:898:17:8000::42";
const int PORT = 23;

// Print message received.
void readout(int sock)
{
    char buffer[BUFSIZ] = {0};
    size_t size;
    while (1)
    {
        size = recv(sock, buffer, BUFSIZ - 1, 0);
        buffer[size] = '\0'; // Make sure to null terminate.
        printf("%s", buffer);
    }
}

// Connect to the server.  This is the bit we really care about.
int run(const char *addr, int port, int ipv)
{
    #ifdef _WIN32
    fprintf(stderr, "Using winsock\n");
    WSDATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return -8;
    SOCKET sock = socket(ipv, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        return -1;
    #else
    fprintf(stderr, "Using unix sockets\n");
        int sock = socket(ipv, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        return -1;
    #endif
    // Connect socket.
    if (ipv == AF_INET)
    {
        // IPv4
        struct sockaddr_in serv;
        serv.sin_family = ipv;
        serv.sin_port = htons(port);
        #ifdef _WIN32
        serv.sin_addr.s_addr = inet_addr(addr);
        #else
        if (inet_pton(ipv, addr, &serv.sin_addr) <= 0)
            return -2;
        #endif
        if (connect(sock, (struct sockaddr *) &serv, sizeof(serv)) < 0)
            return -3;
    }
    else
    {
        // IPv6
        #ifdef _WIN32
        // It would be nice to have a Windows computer on a network that
        // supports IPv6.  Oh well, three for four.
        return -9;
        #else
        struct sockaddr_in6 serv;
        serv.sin6_family = ipv;
        serv.sin6_port = htons(port);
        if (inet_pton(ipv, addr, &serv.sin6_addr) <= 0)
            return -2;
        if (connect(sock, (struct sockaddr *) &serv, sizeof(serv)) < 0)
            return -3;
        #endif
    }
    // Print received data to stdout.
    readout(sock);
    return 0;
}

// Convenience.  Assumes out is at least large enough to hold in.
void str_tolower(const char *in, char *out)
{
    size_t i = 0;
    for (; in[i] != '\0'; i ++)
        out[i] = tolower(in[i]);
    out[i] = '\0';
}

// Convenience.  Figures out which IP version to try.
int ip_ver(char *addr)
{
    size_t n_checked = 0;
    const char *delim = ".";
    char *tok;
    for (tok = strtok(addr, delim); tok != NULL; tok = strtok(NULL, delim))
    {
        if (strlen(tok) > 3)
            return 6;
        for (size_t i = 0; tok[i] != '\0'; i ++)
            if (!isdigit(tok[i]))
                return 6;
        int num = atoi(tok);
        if (num < 0 || num > 255)
            return 6;
        n_checked ++;
    }
    return n_checked == 4 ? 4 : 6;
}

int main(int argc, char **argv)
{
    const char *address = INIT_4;
    int ipv = 4;
    // Handle arguments.
    if (argc == 1)
        fprintf(stderr, "Assuming default IPv4 address: %s\n", INIT_4);
    else
    {
        char *arg = malloc((strlen(argv[1] + 1) * sizeof(char)));
        str_tolower(argv[1], arg);
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "-help") == 0)
        {
            // Print help text.
            puts("Homework 1 TCP client\nby Gregory Gan\n");
            puts("Usage:\thw1 [-h46]\n\thw1 <address>\n");
            puts("Options\n\t-h\thelp\n\t-4\tuse IPv4\n\t-6\tuse IPv6\n");
            puts("Parameters\n\taddress\tuser-provided IP address\n");
            return 0;
        }
        if (strcmp(arg, "-4") == 0 || strcmp(arg, "-v4") == 0)
            fprintf(stderr, "Using default IPv4 address: %s\n", INIT_4);
        else if (strcmp(arg, "-6") == 0 || strcmp(arg, "-v6") == 0)
        {
            fprintf(stderr, "Using default IPv6 address: %s\n", INIT_6);
            address = INIT_6;
            ipv = 6;
        }
        else
        {
            fprintf(stderr, "Using provided address: %s\n", arg);
            address = arg;
            ipv = ip_ver(arg);
            // If it's not a valid IPv4 address, assume IPv6.
            // No, this is not a good way of doing it.  That isn't terribly
            // important for this assignment, though.
            fprintf(stderr, "  Detected IPv%d\n", ipv);
        }
        free(arg);
    }
    // Do the actual network stuff.
    int status = run(address, PORT, ipv == 4 ? AF_INET : AF_INET6);
    const char *output = "";
    // Log exit by return codes.
    switch (status)
    {
        case 0 : output = "OK";                      break;
        case -1: output = "Could not create socket"; break;
        case -2: output = "Invalid address";         break;
        case -3: output = "Connection failed";       break;
        case -8: output = "Winsock init failed";     break;
        case -9: output = "Not yet implemented";     break;
        default: output = "Unknown error";
    }
    fprintf(stderr, "%s\n\n", output);
    return status;
}
