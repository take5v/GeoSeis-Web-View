var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Nedra WebGL' });
});

router.post('/', function(req, res, next) {
	console.log(req);
  	res.send('POST request to the homepage');
});

module.exports = router;
