var cors = require('cors');
var express = require('express');
var randnum = require('random-number-between');

var currentcube = 0;
var currentstate = 0;
var app = express();

app.use(cors());
app.use(express.static(__dirname + '/public/'));

app.get('/', function(req,res) {
      res.send('index.html');
});

app.get('/number', function (req, res) {
      res.json({cube: currentcube, state: currentstate});
});

app.post('/update', function (req, res) {
      //console.log(req.data);
      currentcube = req.query.cube;
      currentstate = req.query.state;
      res.status(200).json({ message: 'Success!' });
});

var port = 8080;
app.listen(port, function () {
      console.log('app listening on port ' + port);
});

function myRandomNumber() {
      var rand = randnum(1, 3, 1);
      return rand;
}

