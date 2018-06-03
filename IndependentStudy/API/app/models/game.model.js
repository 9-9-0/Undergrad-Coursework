var mongoose = require('mongoose');

var GameSchema = mongoose.Schema({
	GameState: 
	{
		State: Number, //0 = awaiting start, 1 = in progress, 2 = ended
		Start: Date,
		End: Date
	},
	GameInfo: 
	{
		Sport: String,
		HomeTeam: String,
		VisitingTeam: String
	},
	GameResults: 
	{
		Victor: String,
		Scores: 
		{
			Final: { H: Number, V: Number }, 
			Progression: [{ H: Number, V: Number, Time: Date }]
		}
	}
}, {
	timestamps: true,
	minimize: false
});

module.exports = mongoose.model('Game', GameSchema);