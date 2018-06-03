var Team = require('../models/team.model.js');
var Sport = require('../models/sport.model.js');
var mongoose = require('mongoose');
var objectIdType = mongoose.Types.ObjectId;

exports.create = function(req, res) {
	if (!req.body.City || !req.body.Sport) {
		return res.status(404).send({message : "A sport and city must be provided to create a team."});
	}
	if (req.body.City.length != 3) {
		return res.status(404).send({message : "City abbreviation must be three characters long."})
	}

	Sport.find({ Name : req.body.Sport.toUpperCase() }).then(sport => {
		if (sport.length != 1) return res.status(404).send({message : "Sport: " + req.body.Sport + " not found."});

		Team.find({Sport: req.body.Sport.toUpperCase(), City: req.body.City}).then(teamCheck => {
			if (teamCheck.length != 0) return res.status(404).send({message : "Team already registered."});
			
			var team = new Team({
				Sport: req.body.Sport.toUpperCase(),
				City: req.body.City.toUpperCase(),
				Wins: 0,
				Losses: 0
			});

			team.save(function(err, data) {
				if (err) {
					console.log(err);
					res.status(500).send({message: "Error when saving team."});
				}
				else {
					res.send(data);
				}
			});	
		}).catch(error => {
			console.log(error);
			return res.status(500).send(error);
		})
	}).catch(error => {
		console.log(error);
		res.status(500).send(error);
	});
};

exports.findAll = function(req, res) {
	 Team.find(function(err, teams){
        if(err) {
            console.log(err);
            res.status(500).send({message: "Some error occurred while retrieving teams."});
        } else {
            res.send(teams);
        }
    });
};

exports.findOne = function(req, res) {
	Team.findById(req.params.teamId, function(err, team) {
        if(err) {
            console.log(err);
            if(err.kind === 'ObjectId') {
                return res.status(404).send({message: "Team not found with id " + req.params.teamId});                
            }
            return res.status(500).send({message: "Error retrieving team with id " + req.params.teamId});
        } 

        if(!team) {
            return res.status(404).send({message: "Team not found with id " + req.params.teamId});            
        }

        res.send(team);
    });
};

exports.delete = function(req, res) {
	Team.remove({_id: objectIdType(req.params.teamId)}).then(qRes => {
		console.log(qRes);
		if (qRes.n != 1) {
			return res.status(404).send({message: "Team with ID " + req.params.teamId + " was not found."});
		}
		else {
			return res.send({message: "Team with ID " + req.params.teamId + " successfully deleted."});
		}
	}).catch(error => {
		console.log(error);
		return res.status(500).send(error);
	})
};

/*
exports.update = function(req, res) {

};


*/