all:
	go build ./clock2.go clock2
	go build ./clockWall.go clockWall
	$(TZ=US/Eastern    ./clock2 -port 8010 & TZ=Asia/Tokyo    ./clock2 -port 8020 & TZ=Europe/London ./clock2 -port 8030 &)

clean:
	($rm) clock2 clockWall
