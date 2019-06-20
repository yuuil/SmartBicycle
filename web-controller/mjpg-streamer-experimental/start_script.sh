export STREAMER_PATH=$HOME/node/esw1/web-controller/mjpg-streamer-experimental
export LD_LIBRARY_PATH=$STREAMER_PATH
$STREAMER_PATH/mjpg_streamer -i "input_raspicam.so -d 200" -o "output_http.so -w $STREAMER_PATH/www"
