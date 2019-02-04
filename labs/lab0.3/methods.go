/*
* Source code taken from: https://github.com/adonovan/gopl.io/blob/master/ch6/geometry/geometry.go
* To create methods we need to use the (p Pointer) notation in every "fuction" to transform it to method of the given struct.
*/

package main

import (
	"fmt"
)

type Point struct{ x, y float64 }

func (p Point) X() float64 {
	return p.x
}

func (p Point) Y() float64 {
	return p.y
}

func (p Point) printPoint(){
	fmt.Printf("%f, %f", p.X(), p.Y())
}

func main(){
	p1 := Point{0.5,0.7}
	p1.printPoint()	
}
