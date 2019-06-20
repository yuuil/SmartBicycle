var express = require('express');
var fs = require('fs');
var bodyParser = require('body-parser');
var cors = require('cors');
var app = express();

app.use(bodyParser.urlencoded({extended:true}));
app.use(bodyParser.json());
app.use(cors());

app.use(express.static('public'));
app.locals.pretty = true
app.set('views', './view_file')
app.set('view engine', 'pug')
app.listen(3000, ()=> {
	    console.log("child server start")
})

app.get("/", (req, res)=> {
	    fs.readFile('index.html', (error, data)=> {
		            if(error) {
				                console.log('error :' + error)
				            }
		            else {
				                res.writeHead(200, {'ContentType':'text/html'})
				                res.end(data)
				            }
		        })
})

app.post("/", function(req, res, next) {
	    var data = req.body.data;

	    
	    fs.open("./child.txt", "w+", function(err, fd) {
		        if(err) throw err;
		        
		        var buf = new Buffer(data); //받아온 데이터 넣어주기.
		        fs.write(fd, buf, 0, buf.length, null, function(err, written, buffer) {
				        if(err) throw err;
				        fs.close(fd, function() {
						            console.log("업데이트 완료");
						        })
				    })
		        })
})
