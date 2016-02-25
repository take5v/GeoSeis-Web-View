var express = require('express');
var router = express.Router();

var scs3reader = require('scs3reader_addon/build/Release/scs3reader_addon');

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Nedra WebGL' });
});

router.post('/', function(req, res, next) {
	console.log(req);
  	res.send('POST request to the homepage');
});

router.get('/data', function(req, res, next) {
	var url = 'public/data/D190794.SCS3';
	var parser = new scs3reader.Seismogram();
	var seis = parser.parse(url);
	res.send(seis);
});

module.exports = router;
