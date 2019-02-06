package main

import (
    "golang.org/x/tour/wc"
    "strings"
)

func WordCount(s string) map[string]int {
    //Crea y prepara el map para poder hacer inserciones
    word_map := make(map[string]int)
    //Itera atraves del array/slice.
    for _ ,word:= range strings.Fields(s){
        //Crea la entrada. Anteriormente fueron las entradas creadas con 0.
        word_map[word]++
    }
    return word_map
}

func main() {
    wc.Test(WordCount)
}
