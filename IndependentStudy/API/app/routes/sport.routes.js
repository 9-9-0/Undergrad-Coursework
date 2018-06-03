module.exports = function(app) {

	//Controller Import
	var sports = require('../controllers/sport.controller.js');

	//Create
	app.post('/sports', sports.create);

	//Get
	app.get('/sports', sports.findAll);

	//Retrieve single team with ID
	app.get('/sports/:sportId', sports.findOne);

	//Delete a Note with ID
	app.delete('/sports/:sportId', sports.delete);
}