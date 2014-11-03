var express = require('express'),				   // make an instance of express
	app = express();		

var Parse = require('parse').Parse;
Parse.initialize("iId3itQJ8VJPKpfWz6CaHsUPWPOczdC1s4f51UBp", "FubhY6tDujYhXP0xtzgLnea5UebqPPPoiqRVezs8");

var ParseData = Parse.Object.extend("ParseData");
var query = new Parse.Query(ParseData);
var parseData = new ParseData();

query.get("HZFWYDYbLt", {
  success: function(pd) {
    // The object was retrieved successfully.
    parseData = pd;
  },
  error: function(object, error) {
    // The object was not retrieved successfully.
    // error is a Parse.Error with an error code and message.
  }
});


var onEveryHour = false;
var onEveryDay = false;
var time = "0";

var port = process.env.PORT || 8080;
app.listen(port);				// start the server listening

app.use('/js', express.static(__dirname + '/js'));

// let the user know you started:
console.log('Server is listening on port 8080');

function turnOnEveryHour( req, res ) {
	var setting = req.params[0];

	if( setting == 'on' ){

		parseData.save( null, {
			success: function( parseData ) {
				parseData.set("onEveryHour", true);
			} 
		});

		//onEveryHour = true;

	} else if( setting == 'off' ) {
		//onEveryHour = false;

		parseData.save( null, {
			success: function( parseData ) {
				parseData.set("onEveryHour", false);
			} 
		});
	} else {

	}

	res.write("The setting is: " + setting + ", and onEveryHour = " + onEveryHour );
	res.end();
}

function turnOnEveryDay( req, res ) {
	var setting = req.params[0];

	if( setting == 'on' ){
		//onEveryDay = true; // for local only
		
		parseData.save( null, {
			success: function( parseData ) {
				parseData.set("onEveryDay", true);
			} 
		});

	} else if( setting == 'off' ) {
		//onEveryDay = false;
		
		parseData.save( null, {
			success: function( parseData ) {

				parseData.set("onEveryDay", false);
			} 
		});
	}

	res.write("The setting is: " + setting + ", and onEveryDay = " + onEveryDay );
	res.end();
}

function checkForHour( req, res ) {

	onEveryHour = parseData.get("onEveryHour");
	res.write( onEveryHour + '' );
	res.end();
}

function checkForDay( req, res ) {

	onEveryDay = parseData.get("onEveryDay");
	res.write( onEveryDay + '' );
	res.end();
}

function setTime( req, res ) {
	time = req.params[0];

	parseData.save( null, {
		success: function( parseData ) {
			parseData.set("time", time);
		} 
	});
	res.write("Time is: " + time);
	res.end();
}

function getTime( req, res ) {
	var d = new Date();
	var h = d.getHours();

	time = parseData.get("time");

	console.log(h);
	console.log(time);

	if( time == h ) {
		res.write('t');
	} else {
		res.write('f');
	}
	res.end();
}

app.get('/', function (request, response) {
   response.sendfile('js/index.html');
      //console.log(request.ip);
});

app.get('/time', getTime );
app.get('/time/*', setTime );

app.get('/hour', checkForHour );
app.get('/day', checkForDay );

app.get('/hour/*', turnOnEveryHour );
app.get('/day/*', turnOnEveryDay );
