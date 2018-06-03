var mongoose = require('mongoose');

var SportSchema = mongoose.Schema({
	Name: String
}, {
	timestamps: true
});

module.exports = mongoose.model('Sport', SportSchema);
