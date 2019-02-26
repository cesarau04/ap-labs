package main

import(
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"sync"
	"io"
	"bytes"
)

var wg sync.WaitGroup

func readConnection(city, socket string) {
	defer wg.Done()
	conn, err := net.Dial("tcp", socket)
	defer conn.Close()
	if err != nil {
		log.Fatal(err)
		return;
	}

	for {
		s := city + ": "
		buf := bytes.NewBufferString(s)
		_, err := io.CopyN(buf, conn, 9)
		if err != nil {
			log.Fatal(err)
			return;
		}
		fmt.Printf("%s", buf.String())
	}
}

func main(){
	if len(os.Args) < 2{
		fmt.Printf("Usage: ./clockWall city=localhost:port\n");
		return;
	}

	wg.Add(len(os.Args)-1)
	for i := 1; i < len(os.Args); i++ {
		s := strings.Split(os.Args[i], "=")
		city, socket := s[0], s[1]
		go readConnection(city, socket)
	}
	wg.Wait()
}
