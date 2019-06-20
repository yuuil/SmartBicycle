var express = require('express'), 
	http = require('http'),
	app = express(),
	server = http.createServer(app) ;

app.use(express.static(__dirname + '/images')) ;
var img_flag = 0 ;

var cameraOptions = {
	  width : 600,
	  height : 420,
	  mode : 'timelapse',
	  awb : 'off',
	  encoding : 'jpg',
	  output : 'images/camera.jpg',
	  q : 50,
	  timeout : 10000000000000,
	  timelapse : 0,
	  nopreview : true,
	  th : '0:0:0'
};

var camera = new require('raspicam')(cameraOptions) ;
camera.start() ;

camera.on('exit', function() {
	    camera.stop() ;
	    console.log('Restart camera') ;
	    camera.start() ;
	  }) ;

camera.on('read', function() {
	    img_flag = 1 ;
	  }) ;

app.get('/', function(req, res) {
	    res.sendfile('index.html', {root : __dirname}) ;
	  }) ;

app.get('/img', function (req, res) {
	console.log('get /img') ;
	if (img_flag == 1) {
		img_flag = 0 ;
		res.sendfile('images/camera.jpg') ;
	}
}) ;

var fs = require('fs');
var bodyParser = require('body-parser');

app.use(bodyParser.urlencoded({extended:true}));
app.use(bodyParser.json());

app.use(express.static('public'));
app.locals.pretty = true
app.set('views', './view_file')
app.set('view engine', 'pug')
app.listen(3000, ()=> {
    console.log("server start")
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
    //console.log(data);
    
    fs.open("./data.txt", "w+", function(err, fd) {
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

//module.exports = router;
//ajax로 받아온 데이터로 파일 만들 때 이용
/*fs.open("./data.txt", "w+", function(err, fd) {
    if(err) throw err;
    
    var buf = new Buffer("안녕\n"); //받아온 데이터 넣어주기.
    fs.write(fd, buf, 0, buf.length, null, function(err, written, buffer) {
        if(err) throw err;
        fs.close(fd, function() {
            console.log("완료");
        })
    })
})*/
