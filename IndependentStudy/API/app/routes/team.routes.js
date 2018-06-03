module.exports = function(app) {

	//Controller Import
	var teams = require('../controllers/team.controller.js');

	//Create
	app.post('/teams', teams.create);

	//Get
	app.get('/teams', teams.findAll);

	//Retrieve single team with ID
	app.get('/teams/:teamId', teams.findOne);

	//Delete a Note with ID
	app.delete('/teams/:teamId', teams.delete);
	
	/*
	//Update a Note with ID
	app.put('/teams/:teamId', teams.update);

	*/
}