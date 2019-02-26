package main

import(
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"sync"
	"io"
)

var wg sync.WaitGroup

func readConnection(city, socket string) {
	defer wg.Done()

	conn, err := net.Dial("tcp", socket)
	defer conn.Close()
	if err != nil {
		log.Fatal(err)
	}

	for {
		fmt.Printf("%s: ", city)
		_, err := io.CopyN(os.Stdout, conn, 9)
		if err != nil {
			log.Fatal(err)
		}
	}
}

func main(){
	if len(os.Args) < 2{
		fmt.Printf("Usage: ./clockWall city=localhost:port");
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
