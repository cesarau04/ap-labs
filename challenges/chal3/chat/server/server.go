// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"net"
	"regexp"
	"strconv"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

var (
	clientsConnected int
	entering         = make(chan client)
	leaving          = make(chan client)         // channel for leaving clients
	messages         = make(chan string)         // all incoming client messages
	usersChannel     = make(map[string]net.Conn) //Here come the connection per user
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			clientsConnected--
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)
	clientsConnected++
	who := "user" + strconv.Itoa(clientsConnected)
	usersChannel[who] = conn
	ch <- "!< " + who + " >!"
	messages <- "irc-server > [" + who + "] has arrived"
	entering <- ch
	fmt.Printf("irc-server > New connected user [%s]\n", who)

	input := bufio.NewScanner(conn)
	for input.Scan() {
		msg := input.Text()
		fmt.Printf("DATA: %s\n", msg)
		if msg == "/users" {
			usersStr := "| "
			for i := 1; i < clientsConnected+1; i++ {
				usersStr += "user" + strconv.Itoa(i) + " | "
			}
			fmt.Fprintf(conn, "irc-server > %s\n", usersStr)
		} else if msg == "/time" {
			fmt.Fprintf(conn, "irc-server > %s\n", time.Now().Format("Mon Jan _2 15:04:05 2006"))
		} else if matches, _ := regexp.MatchString("^/msg user[0-9]+ .+", msg); matches {
			stringSlice := strings.Split(msg, " ")
			numberAsked := strings.Split(stringSlice[1], "r")
			if value, _ := strconv.Atoi(numberAsked[1]); value > len(usersChannel) {
				fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			} else {
				lenSlice := len(stringSlice)
				fmt.Println(stringSlice[1])
				fmt.Printf("%T\n", stringSlice[1])
				fmt.Println(lenSlice)
				fmt.Fprintf(usersChannel[stringSlice[1]], "%s [private] > ", who)
				for i := 2; i < lenSlice; i++ {
					fmt.Fprintf(usersChannel[stringSlice[1]], "%s ", stringSlice[i])
				}
			}
			fmt.Fprintf(usersChannel[stringSlice[1]], "\n")
		} else if matches, _ := regexp.MatchString("^/user user[0-9]+$", msg); matches {
			stringSlice := strings.Split(msg, " ")
			numberAsked := strings.Split(stringSlice[1], "r")
			if value, _ := strconv.Atoi(numberAsked[1]); value > len(usersChannel) {
				fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			} else {
				fmt.Fprintf(conn, "irc-server > name: %s, ip: %s\n", stringSlice[1], usersChannel[stringSlice[1]].RemoteAddr().String())
			}
		} else if matches, _ := regexp.MatchString("^<meta>.+", msg); matches {
			fmt.Printf("User received==>data: %s\n", msg)
			stringSlice := strings.Split(msg, ">")
			fmt.Printf("--> %s", stringSlice[1])
		} else {
			messages <- who + "> " + msg
		}
	}
	// NOTE: ignoring potential errors from input.Err()

	leaving <- ch
	messages <- "irc-server > [" + who + "] left"
	fmt.Printf("irc-server > [%s] left\n", who)
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	host := flag.String("host", "localhost", "")
	port := flag.String("port", "9000", "")
	flag.Parse()
	listener, err := net.Listen("tcp", *host+":"+*port)
	fmt.Printf("irc-server > Simple IRC Server started at %s:%s\n", *host, *port)
	if err != nil {
		log.Fatal(err)
	}

	clientsConnected = 0
	go broadcaster()
	fmt.Printf("irc-server > Ready for receiving new clients\n")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
