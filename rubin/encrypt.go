package main

import (
	"./cryptosystem"
	"bufio"
	"fmt"
	"log"
	"math/big"
	"os"
)

func main() {
	reader := bufio.NewReader(os.Stdin)

	fmt.Println("let's play with encryption")
	fmt.Println("Please, enter server's n:")
	var s_pub [2]*big.Int
	s_pub_n, err := reader.ReadString('\n')
	s_pub[0] = big.NewInt(0)
	s_pub[0].SetString(s_pub_n, 16)
	fmt.Println("Please, enter server's b:")
	s_pub_b, err := reader.ReadString('\n')
	s_pub[1] = big.NewInt(0)
	s_pub[1].SetString(s_pub_b, 16)

	fmt.Println("Please, enter message")
	msg, err := reader.ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}

	cryptogram, c1, c2 := cryptosystem.Encrypt([]byte(msg), s_pub)
	fmt.Printf("[%X]\n", cryptogram)
	fmt.Println(c1)
	fmt.Println(c2)
}
