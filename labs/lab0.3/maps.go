package main

import (
    "golang.org/x/tour/wc"
    "strings"
)

func WordCount(s string) map[string]int {
    word_map := make(map[string]int)
    for _ ,word:= range strings.Fields(s){
        word_map[word]++
    }
    return word_map
}

func main() {
    wc.Test(WordCount)
}
