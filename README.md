# SmartBicycle 사용법



<실행 매뉴얼>

①	PI-A 를 가장 먼저 insmod 한 뒤, 1) 카메라 서버, 2) parent 서버, 3)child 서버 순으로 순차적으로 서버를 올린다.

②	두 개의 탭을 열어 각각 http://192.168.0.7:3000 (child 서버), http://192.168.0.7:3001 (parent 서버) 을 접속한다. (현재 프로젝트의 IP는 192.168.0.7로 고정되어 있으므로 자신의 IP나 로컬호스트로 접속하도록 하려면 html과 js 파일의 URL 부분을 수정해야 한다.)

③	mjpg-streamer를 설치한 폴더에서 start_script.sh 의 export STREAMER_PATH 를 sh 파일이 저장되어있는 폴더까지의 경로로 수정한다.


<사용 매뉴얼>

① child 서버에서 버튼을 눌러 유아의 행동을 시뮬레이션한다. 

② parent 서버에서 lock을 누른 뒤 child 서버로 돌아가 버튼을 누르면 lock 되었다는 문구와 함께 child 서버에서 기기 제어가 불가능하다. 

③ parent 서버에서 warning 버튼을 누르면 lock이 함께 작동하면서 경보음이 울린다. 

④ parent 서버에서 좌,우, 정지 버튼을 눌러서 기기에 부착된 카메라의 회전이 가능하다. 
