var Game = require('../models/game.model.js');
var Team = require('../models/team.model.js');
var Sport = require('../models/sport.model.js');


exports.create = function(req, res) {
	if (!req.body.Sport || !req.body.HomeTeam || !req.body.VisitingTeam) {
		return res.status(400).send({message : "Game must have a home team, visiting team, and sport."});
	}

	Sport.find({Name : req.body.Sport.toUpperCase()}).then(sportCheck => {
		if (sportCheck.length != 1) return res.status(404).send({message : "Invalid sport entered."});

		Team.find({Sport : req.body.Sport.toUpperCase(), City : req.body.HomeTeam.toUpperCase()}).then(homeCheck => {
			if (homeCheck.length != 1) return res.status(404).send({message : "Invalid home team entered."});

			Team.find({Sport : req.body.Sport.toUpperCase(), City :req.body.VisitingTeam.toUpperCase()}).then(visitCheck => {
				if (visitCheck.length != 1) return res.status(404).send({message : "Invalid visitng team entered."});
				
				var game = new Game({
					GameState: 
					{
						State: 0,
						Start: null,
						End: null
					},
					GameInfo: 
					{
						Sport: req.body.Sport.toUpperCase(),
						HomeTeam: req.body.HomeTeam.toUpperCase(),
						VisitingTeam: req.body.VisitingTeam.toUpperCase()
					},
					GameResults:
					{
						Victor: null,
						Scores: 
						{
							Final: null
						}
					}
				});

				game.save(function(err, data) {
					if (err) {
						console.log(err);
						res.status(500).send({message: "Error saving game."});
					}
					else {
						res.send(data);
					}
				});
			})
		})
	})
};

exports.findAll = function(req, res) {
 	Game.find(function(err, games){
        if(err) {
            console.log(err);
            res.status(500).send({message: "Some error occurred while retrieving games."});
        } else {
            res.send(games);
        }
    });
};

exports.findOne = function(req, res) {
	Game.findById(req.params.gameId, function(err, game) {
        if(err) {
            console.log(err);
            if(err.kind === 'ObjectId') {
                return res.status(404).send({message: "Game not found with id " + req.params.gameId});                
            }
            return res.status(500).send({message: "Error retrieving game with id " + req.params.gameId});
        } 

        if(!game) {
            return res.status(404).send({message: "Game not found with id " + req.params.gameId});            
        }

        res.send(game);
    });
};

exports.delete = function(req, res) {
	/*
	- Functionality to be designed later...
	*/
};

exports.beginGame = function(req, res) {
	if (!req.params.gameId) {
		return res.status(400).send({message: "GameId missing in parameters."});
	}

	Game.findById(req.params.gameId, function(err, game) {
		if(err) {
            console.log(err);
            if(err.kind === 'ObjectId') {
                return res.status(404).send({message: "Game not found with id " + req.params.gameId});                
            }
            return res.status(500).send({message: "Error retrieving game with id " + req.params.gameId});
        } 

        if(!game) {
            return res.status(404).send({message: "Game not found with id " + req.params.gameId});            
        }

        if (game.GameState.State != 0) {
        	return res.status(404).send({message: "Game has already ended or is in play."});
        }

        game.GameState.State = 1;		

        game.save(function(err, data) {
			if (err) {
				console.log(err);
				res.status(500).send({message: "Error saving game."});
			}
			else {
				res.status(200).send({message: "Game is now in play."});
			}
		});
	});
}
exports.simulateGame = function(req, res) {
	/*
	- Randomizes points scored by home and visiting team for each quarter until all four have been scored.
	*/
	if (!req.body.id || !req.body.range || !req.body.advantage) {
		return res.status(400).send({message : "Must have fields: id, range, and advantage"});
	}

	var scoreRange = parseInt(req.body.range);
	var advantage = Math.floor(parseInt(req.body.advantage)); 
	//[-100, 100], 0 is 50-50 chance of winning, 1 = home is twice as likely, -1 = visiting team twice as likely

	if (advantage > 99 || advantage < -99) {
		return res.status(400).send({message : "Advantage must be between -100 and 100."});
	}

	var homeFinalScore;
	var visitFinalScore;

	Game.findById(req.body.id, function(err, game) {
		if (err) {
			console.log(err);
			if (err.kind === 'ObjectId') {
				return res.status(404).send({message: "Game not found with id " + req.body.id});
			}
            return res.status(500).send({message: "Error retrieving game with id " + req.body.id});
		}

        if(!game) {
            return res.status(404).send({message: "Game not found with id " + req.body.id});            
        }

        if (game.GameState.State != 1) {
        	return res.status(404).send({message: "Game " + req.body.id + " is not currently in play."});
        }

		if (advantage < 0) {
			var t = advantage + 1;
			var p = 100 / t;
			var homeWin = Math.random() * 100;


			visitFinalScore = Math.floor(Math.random() * scoreRange);
			if (homeWin > p) {
				homeFinalScore = Math.floor(Math.random() * (scoreRange - visitFinalScore) + visitFinalScore);
			}
			else {
				homeFinalScore = Math.floor(Math.random() * visitFinalScore);
			}

		}
		else if (advantage > 0) {
			var t = advantage + 1;
			var p = 100 / t;
			var visitWin = Math.random() * 100;

			homeFinalScore = Math.floor(Math.random() * scoreRange);
			if (visitWin > p) {
				visitFinalScore = Math.floor(Math.random() * (scoreRange - homeFinalScore) + homeFinalScore);
			}
			else {
				visitFinalScore = Math.floor(Math.random() * homeFinalScore);
			}
		}
		else {
			homeFinalScore = Math.floor(Math.random() * scoreRange);
			visitFinalScore = Math.floor(Math.random() * scoreRange);
		}

		console.log(game);

		game.GameResults.Scores.Final.H = homeFinalScore;
		game.GameResults.Scores.Final.V = visitFinalScore;

		if (homeFinalScore > visitFinalScore) {
			game.GameResults.Victor = "H";
		}
		else if (homeFinalScore < visitFinalScore) {
			game.GameResults.Victor = "V";
		}
		else {
			game.GameResults.Victor = "T";
		}

		game.GameState.State = 2;

		game.save(function(err, data) {
			if (err) {
				console.log(err);
				res.status(500).send({message: "Error saving game."});
			}
			else {
				res.send(data);
			}
		});
	})



}