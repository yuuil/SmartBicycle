var express = require('express');
var fs = require('fs');
var bodyParser = require('body-parser');
var cors = require('cors');
var app = express();
/*var async = require('async');
var q = async.queue(function(task, callback) {
    callback();
}, 500)*/

app.use(bodyParser.urlencoded({extended:true}));
app.use(bodyParser.json());
app.use(cors());
app.use(express.static('public'));
app.locals.pretty = true
app.set('views', './view_file')
app.set('view engine', 'pug')


//서버 열리면 lock false로 초기화하여 파일에 저장
app.listen(3001, ()=> {
    console.log("parent server start");
    var obj =  {
        lock: "false"
    }
    var temp = "0\n0\n0\n0";
    var json = JSON.stringify(obj);
    
    fs.writeFile('parent.txt', temp, 'utf8', function(error) {
	if(error) {
	    console.log("파일등록 실패", error);
	}
	else {
	    console.log("초기값 저장");
	}
    });
    fs.writeFile('parent.json', json, 'utf8',function(error){
        if(error){
            console.log("파일등록 실패",error);
        }
        else {
             /**
              파일쓰기 완료 콜백
            */
            console.log("파일저장");
        }
    });
})

//3001 포트의 루트로 접속 시
app.get("/", function(req, res, next) {
	   //   var date = new date();
/*
	      res.writehead(200, {
		                      'date': date.toutcstring(),
		                      'connection': 'close',
		                      'cache-control': 'private',
		                      'content-type': 'video/webm',
		                      'server': 'customstreamer/0.0.1',
		                 });
	      var net = require('net');
	      var socket = net.connect(9001, function () {
		                      socket.on('close', function (had_error) {
					                              res.end();
					                          });
		                      socket.on('data', function (data) {
					                              res.write(data);
					                          });
		                  });
	      socket.on('error', function (error) {
		                      console.log(error);
		                  });
*/
    fs.readFile('parent.html', (error, data)=> {
        if(error) {
            console.log('error :' + error)
        }
        else {
            res.writeHead(200, {'ContentType':'text/html'})


            res.end(data)
        }
    })
})

//부모 컨트롤러의 버튼값 텍스트 파일에 저장
app.post("/", function(req, res, next) {
    var data = req.body.data;
    console.log(data);

    var buf = Buffer.alloc(10);
    buf = Buffer.from(data);
    /*fs.open("./parent.txt", "w+", function(err, fd) {
        if(err) throw err;
        var buf = new Buffer(data); //받아온 데이터 넣어주기.
        fs.write(fd, buf, 0, buf.length, null, function(err, written, buffer) {
            if(err) throw err;
            fs.close(fd, function() {
                console.log("업데이트 완료");
            })
        })
    })*/
    
    fs.writeFile("./parent.txt", buf, 'utf8', function(err) {
        if(err) throw err;
        console.log("성공")
        res.sendStatus(200);
    });
    
    /*function asyncTask(callback) {
        process.nextTick(callback);
    }
    
    function writing(callback) {
        fs.writeFileSync("./parent.txt", buf, 'utf8');
    }
    
    q.push( writing(()=> {console.log("쓰기")}), function(err) {
        console.log("success");
    })*/
    
})

//lock 버튼값 json으로 저장
app.post("/parent", function(req, res, next) {
    var data = req.body.chk;
    var obj =  {
        lock: data
    }
    
    var json = JSON.stringify(obj);
    
    fs.writeFile('parent.json', json, 'utf8',function(error){
        if(error){
            console.log("파일등록 실패",error);
        }
        else {
//             /**
//              파일쓰기 완료 콜백
//            */
//            fs.readFile('parent.json', 'utf8', function readFileCallback(err, data){
//                /*
//                    파일 읽기 완료 콜백
//                */
//                if (err){
//                    console.log(err);
//                } else {
//                    res.writeHead(200, {'ContentType': 'application/json'})
//                    res.end(data)
//            }});
            res.writeHead(200, {'ContentType': 'application/json'});
            res.end(data);
        }
    });
})

//저장된 lock 값 json 형태로 출력
app.get("/parent", function(req, res, next) {
    fs.readFile('parent.json', 'utf8', function readFileCallback(err, data){
        /*
            파일 읽기 완료 콜백
        */
        if (err){
            console.log(err);
        } else {
            res.writeHead(200, {'ContentType': 'application/json'})
            res.end(data)
    }});
})
/*

app.get('/', function (req, res) {
	      var date = new date();

	      res.writehead(200, {
		                      'date': date.toutcstring(),
		                      'connection': 'close',
		                      'cache-control': 'private',
		                      'content-type': 'video/webm',
		                      'server': 'customstreamer/0.0.1',
		                 });
	      var net = require('net');
	      var socket = net.connect(9001, function () {
		                      socket.on('close', function (had_error) {
					                              res.end();
					                          });
		                      socket.on('data', function (data) {
					                              res.write(data);
					                          });
		                  });
	      socket.on('error', function (error) {
		                      console.log(error);
		                  });
}o\); */
