package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	//make receives three paramenters; type, length, capacity.
	matrix := make([][]uint8, dy, dy)
	
	//iterate in matrix, we use i for getting the index, we can use _ to get the value
	//but we have no values store so...
	//we need to iterate throu every pair (i,j) so that's why we have to for
	for i := range matrix {
		//we create an array inside an array
		matrix[i] = make([]uint8, dx)
		for j := range matrix {
			matrix[i][j] = uint8(i^j)
		}
	}
	return matrix
}

func main() {
	pic.Show(Pic)
}
