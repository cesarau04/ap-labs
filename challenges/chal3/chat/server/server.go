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
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

var (
	entering     = make(chan client)
	leaving      = make(chan client)         // channel for leaving clients
	messages     = make(chan string)         // all incoming client messages
	usersChannel = make(map[string]net.Conn) //Here come the connection per user
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
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)
	who := ""

	input := bufio.NewScanner(conn)
	for input.Scan() {
		msg := input.Text()

		if msg == "/users" {
			fmt.Fprintf(conn, "irc-server > ")
			for key := range usersChannel {
				fmt.Fprintf(conn, "| %s |", key)
			}
			fmt.Fprintln(conn, "")
		} else if msg == "/time" {
			fmt.Fprintf(conn, "irc-server > %s\n", time.Now().Format("Mon Jan _2 15:04:05 2006"))
		} else if matches, _ := regexp.MatchString("^/msg .+ .+", msg); matches {
			stringSlice := strings.Split(msg, " ")
			lenSlice := len(stringSlice)
			if connection, ok := usersChannel[stringSlice[1]]; ok {
				fmt.Fprintf(connection, "%s [privatemsg] > ", who)
				for i := 2; i < lenSlice; i++ {
					fmt.Fprintf(connection, "%s ", stringSlice[i])
				}
				fmt.Fprintln(connection, "")
			} else {
				fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			}
		} else if matches, _ := regexp.MatchString("^/user .+$", msg); matches {
			stringSlice := strings.Split(msg, " ")
			if _, ok := usersChannel[stringSlice[1]]; ok {
				fmt.Fprintf(conn, "irc-server > name: %s, ip: %s\n", stringSlice[1], usersChannel[stringSlice[1]].RemoteAddr().String())
			} else {
				fmt.Fprintf(conn, "irc-server > No such user; use /users to see actual users\n")
			}
		} else if matches, _ := regexp.MatchString("^<meta>.+", msg); matches {
			// check if user is already picked!
			stringSlice := strings.Split(msg, ">")
			who = stringSlice[1]
			messages <- "irc-server > [" + who + "] has arrived"
			usersChannel[who] = conn
			ch <- "irc-server > Your user [" + who + "] is successfully logged"
			entering <- ch
			fmt.Printf("irc-server > New connected user [%s]\n", who)
		} else {
			messages <- who + "> " + msg
		}
	}
	// NOTE: ignoring potential errors from input.Err()

	leaving <- ch
	messages <- "irc-server > [" + who + "] left"
	fmt.Printf("[%s] left\n", who)
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
