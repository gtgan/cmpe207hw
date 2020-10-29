'use strict'
//this is explicity in the lambda editor. 
const AWS = require('aws-sdk');
//us-west-1 for NorCal
AWS.config.update({region: 'us-east-1'});

exports.handler = async (event, context) => {
    const ddb = new AWS.DynamoDB({apiVersion: "2012-10-08"});
    const documentClient = new AWS.DynamoDB.DocumentClient({region: 'us-east-1'});
    
    const params = {
        TableName: "Users",
        Item: {
            id: "2",
            firstname: "Karanbir",
            lastname: "Bains"
        }
    }

    try {
        const data = await documentClient.put(params).promise(); 
        console.log(data);
    }catch (err){
        console.log(err);
    }
    
}