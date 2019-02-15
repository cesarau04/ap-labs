package main

import (
	"fmt"
	"os"
	"path/filepath"
)

var directories int = 0
var denied int = 0
var symbolicLink int = 0
var others int = 0

func visit(dir string, f os.FileInfo, err error) error {
	/*
	* References took from Go Documentation
	* https://golang.org/doc/* 
	*/
	if f.Mode()&os.ModeSymlink != 0 {
		symbolicLink++
		return nil;
	}
	if f.IsDir() {
		directories++
		return nil;
	}
	/*
	* Thanks to 'putu' for giving the answer.
	* https://stackoverflow.com/questions/45429210/how-do-i-check-a-files-permissions-in-linux-using-go
	*/
	if f.Mode()&(1<<2) != 0 {
	} else {
    denied++
		return nil;
	}
	if f.Mode().IsRegular() {
		others++
		return nil;
	}
	return nil
}

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
  err := filepath.Walk(dir, visit);
	return err
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	scanDir(os.Args[1])
	fmt.Printf("Path\t\t\t%s\n" +
							"Directories:\t\t%d\n" +
							"Symbolic Links:\t\t%d\n" +
							"Denied:\t\t\t%d\n" +
							"Other files:\t\t%d\n",os.Args[1], directories, symbolicLink, denied,others)
}
