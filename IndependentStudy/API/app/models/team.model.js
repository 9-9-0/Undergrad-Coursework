var mongoose = require('mongoose');

var TeamSchema = mongoose.Schema({
	Sport: String, 
	City: String,
	Wins: Number,
	Losses: Number
}, {
	timestamps: true
});

module.exports = mongoose.model('Team', TeamSchema);

