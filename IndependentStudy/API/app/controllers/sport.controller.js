var Sport = require('../models/sport.model.js');
var mongoose = require('mongoose');
var objectIdType = mongoose.Types.ObjectId;

exports.create = function(req, res) {
	if (!req.body.Name) {
		return res.status(400).send({message : "Sport name can not be empty."});
	}

    Sport.find({ Name : req.body.Name.toUpperCase() }).then(sportObj => {
		if (sportObj.length != 0) {
	    	return res.status(404).send({message: req.body.Name + " already exists."});
	    }
	    var sport = new Sport({
			Name: req.body.Name.toUpperCase()
		});
		sport.save(function(err, data) {
			if (err) {
				console.log(err);
				res.status(500).send({message: "Error when saving team."});
			}
			else {
				res.send(data);
			}
		});
    }).catch(error => {
    	res.status(500).send(error);
    });
};

exports.findAll = function(req, res) {
	Sport.find().then(sports => {
		res.send(sports);
	}).catch(error => {
		console.log(error);
		res.status(500).send({message: "Some error occurred while retrieving sports."});
	})
};

exports.findOne = function(req, res) {
	Sport.findById(req.params.sportId).then(sport => {
        return res.send(sport);
	}).catch(error => {
		if (error.kind == 'ObjectId') {
            return res.status(404).send({message: "Sport not found with id " + req.params.sportId});                
		}
		return res.status(500).send(error);
	});
};

exports.delete = function(req, res) {
	Sport.remove({_id: objectIdType(req.params.sportId)}).then(qRes => {
		console.log(qRes);
		if (qRes.n != 1) {
			return res.status(404).send({message: "Sport with ID " + req.params.sportId + " was not found."});
		}
		else {
			return res.send({message: "Sport with ID " + req.params.sportId + " successfully deleted."});
		}
	}).catch(error => {
		console.log(error);
		return res.status(500).send(error);
	})
};