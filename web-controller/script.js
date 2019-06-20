var cmd = 'gst-launch-1.0';
var args = ['autovideosrc', 'horizontal-speed=1', 'is-live=true',
	    '!', 'videoconvert',
	    '!', 'vp8enc', 'cpu-used=0', 'deadline=1', 'keyframe-max-dist=10',
	    '!', 'queue', 'leaky=1',
	    '!', 'm.', 'webmmux', 'name=m', 'streamable=true',
	    '!', 'queue', 'leaky=1',
	    '!', 'tcpserversink', 'host=127.0.0.1', 'port=5000', 'sync-method=2'];


var child = require('child_process');
var gstreamer = child.spawn(cmd, args, {stdio: 'inherit'});

gstreamer.on('exit', function (code) {
	    if (code != null) {
		            console.log('GStreamer error, exit code ' + code);
		        }
	    process.exit();
});

var express = require('express')
var app = express();
var http = require('http')
var httpServer = http.createServer(app);


app.get('/', function (req, res) {
	    var date = new Date();

	    res.writeHead(200, {
		            'Date': date.toUTCString(),
		            'Connection': 'close',
		            'Cache-Control': 'private',
		            'Content-Type': 'video/webm',
		            'Server': 'CustomStreamer/0.0.1',
		       });

	    var net = require('net');
	    var socket = net.connect(5001, function () {
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
});

httpServer.listen(8001);
