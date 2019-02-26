all:
	go build clock2.go 
	go build clockWall.go
	TZ=US/Eastern    ./clock2 -port 8010 & TZ=Asia/Tokyo    ./clock2 -port 8020 & TZ=Europe/London ./clock2 -port 8030 &

clean:
	($rm) clock2 clockWall
