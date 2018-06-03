module.exports = function(app) {

	//Controller Import
	var games = require('../controllers/game.controller.js');

	//Game Administration
	app.post('/games', games.create);
	app.get('/games', games.findAll);
	app.get('/games/:gameId', games.findOne);
	app.delete('/games/:gameId', games.delete);

	//Mock Game Progression
	app.get('/games/start/:gameId', games.beginGame);
	app.post('/games/simulate', games.simulateGame);
}