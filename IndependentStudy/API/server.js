var express = require('express');
var bodyParser = require('body-parser');

var app = express();

app.use(bodyParser.urlencoded({ extended: true}));

app.use(bodyParser.json());

//Database Configuration
var dbConfig = require('./config/database.config.js');
var mongoose = require('mongoose');

mongoose.Promise = global.Promise;

mongoose.connect(dbConfig.url, {dbName: dbConfig.bet_db});
mongoose.connection.on('error', function() {
	console.log('Connection to ' + dbConfig.url + ' failed. Exiting now...');
	process.exit();
});

mongoose.connection.on('open', function() {
	console.log('Connection to ' + dbConfig.url + ' succeeded.');
});





app.get('/', function(req, res) {
	res.json({"message": "Ayy"});
});

//ROUTES
//require('./app/routes/note.routes.js')(app);
require('./app/routes/game.routes.js')(app);
require('./app/routes/team.routes.js')(app);
require('./app/routes/sport.routes.js')(app);


app.listen(3000, function() {
	console.log("Server is listening on port 3000");
});