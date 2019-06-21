# SmartBicycle 사용법

①	PI-A 를 가장 먼저 insmod 한 뒤, 1) 카메라 서버, 2) parent 서버, 3)child 서버 순으로 순차적으로 서버를 올린다.

②	두 개의 탭을 열어 각각 http://192.168.0.7:3000 (child 서버), http://192.168.0.7:3001 (parent 서버) 을 접속한다. (현재 프로젝트의 IP는 192.168.0.7로 고정되어 있으므로 자신의 IP나 로컬호스트로 접속하도록 하려면 html과 js 파일의 URL 부분을 수정해야 한다.)

③	mjpg-streamer를 설치한 폴더에서 start_script.sh 의 export STREAMER_PATH 를 sh 파일이 저장되어있는 폴더까지의 경로로 수정한다.
