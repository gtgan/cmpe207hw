import json

print('Loading function')

def lambda_handler(event, context):
    #1. Parse out query string paramerters
    transactionId = event['queryStringParameters']['transactionId']
    transactionType = event['queryStringParameters']['transactionType']
    transactionAmount = event['queryStringParameters']['transactionAmount']


    print(event('transactionId= ' + transactionId))
    print(event('transactionType= ' + transactionType))
    print(event('transactionAmount= ' + transactionAmount))

    #2. Construct the body of the response object
    trasnsactionResponse = {}
    trasnsactionResponse['transactionId'] = transactionId
    trasnsactionResponse['type'] = transactionType
    trasnsactionResponse['amount'] = transactionAmount
    trasnsactionResponse['message'] = 'Hello from lambda'

    #3. Construct http response object and pass the response object as the body
    responseObject = {}
    responseObject['statusCode'] = 200
    responseObject['headers'] = {}
    responseObject['headers']['Content-Type'] = 'application/json'
    responseObject['body'] = json.dumps(trasnsactionResponse)