'use strict'
//this is explicity in the lambda editor. 
const AWS = require('aws-sdk');
//us-west-1 for NorCal
AWS.config.update({region: 'us-east-1'});



exports.handler = function (event, context, callback) {
    const ddb = new AWS.DynamoDB({apiVersion: "2012-10-08"});
    const documentClient = new AWS.DynamoDB.DocumentClient({region: 'us-east-1'});
    const params = {
        Key: {
            id: "1"
        }
    }

    documentClient.get(params, (err,data) => {
        if(err) {
            console.log(err);
        }
        console.log(data);
    })
}