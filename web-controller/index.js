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
    var temp = "0\n0\n0\n0";
    fs.writeFile('child.txt', temp, 'utf8', function(err) {
	if(err) {
	    console.log('error :' + error)
	}
	else {
	    console.log("초기값 저장");
	}
    });
    fs.readFile('index.html', (error, data)=> {
        if(error) {
            console.log('error :' + error)
        }
        else {
            res.writeHead(200, {'ContentType':'text/html'})
            res.end(data)
        }
    });
})

app.post("/", function(req, res, next) {
    var data = req.body.data;
    //console.log(data);
    
    fs.open("./child.txt", "w+", function(err, fd) {
    if(err) throw err;
    
    var buf = Buffer.alloc(10); //받아온 데이터 넣어주기.
        buf = Buffer.from(data);
    fs.write(fd, buf, 0, buf.length, null, function(err, written, buffer) {
        if(err) throw err;
        fs.close(fd, function() {
            console.log("업데이트 완료");
            res.sendStatus(200);
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
