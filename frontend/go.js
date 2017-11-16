var AWS = require("aws-sdk");
var express = require('express');
var app = express();
var path = require('path');

AWS.config.update({
  region: "eu-west-2"
});

// Create DynamoDB document client
var documentClient = new AWS.DynamoDB.DocumentClient({apiVersion: '2012-08-10'});
var dynamodb = new AWS.DynamoDB();

console.log("query...");
var table = "sigfox";

var params = {
  ExpressionAttributeValues: {
    ':deviceid': {S: '1D1965'}
  },
  KeyConditionExpression: 'deviceid = :deviceid',
  Limit: 1,
  ScanIndexForward: false,
  TableName: 'sigfox'
};
app.use(express.static(path.join(__dirname, 'public')));

// respond with "hello world" when a GET request is made to the homepage
app.get('/plantdata', function(req, res) {
	dynamodb.query(params, function(err, data) {
	res.setHeader('Content-Type', 'application/json');
  	if (err) {
    		console.log("Error", err);
		res.send("{ status: error; msg: " + err + "}");
  	} else {
    		data.Items.forEach(function(element, index, array) {
        		console.log(element.timestamp.S + " (" + element.deviceid.S + ")");
			var theData = element.payload.M.data.S;
			var token = theData.split("3b");
			if( token.length > 0 ) {
				var sensor1 = 0,sensor2 = 0,sensor3 = 0;
				sensor1 = ((parseInt(token[0].substring(0,2)) - 30) * 10) +  parseInt(token[0].substring(2,4)) - 30;
				console.log("-->"+token[1].length);
				if(token.length>1 && token[1].length > 0)
					sensor2 = ((parseInt(token[1].substring(0,2)) - 30) * 10) +  parseInt(token[1].substring(2,4)) - 30;
				if(token.length>2 && token[2].length > 0)
					sensor3 = ((parseInt(token[2].substring(0,2)) - 30) * 10) +  parseInt(token[2].substring(2,4)) - 30;
				var oneDevice = new Object();
				oneDevice.device = element.deviceid.S;
				oneDevice.sensor1 = sensor1;
				oneDevice.sensor2 = sensor2;
				oneDevice.sensor3 = sensor3;
				var allDevices = new Array();
				allDevices.push(oneDevice);
				var result = new Object();
				result.devices = allDevices;
				result.status = 'ok';
				res.send(result);
			}	
    		});
  	}
	});

});
//



app.listen(3000, () => console.log('Example app listening on port 3000!'))
