package main

import (
	"./cryptosystem"
	"bufio"
	"fmt"
	"log"
	"math/big"
	"os"
	"strings"
)

func main() {
	reader := bufio.NewReader(os.Stdin)

	fmt.Println("Let's play with the verification")

	fmt.Println("Please, enter server's n:")
	var s_pub [2]*big.Int
	s_pub_n, err := reader.ReadString('\n')
	if err != nil {
		log.Fatal(err)
	}
	s_pub[0] = big.NewInt(0)
	s_pub[0].SetString(s_pub_n, 16)

	fmt.Println("Please enter the signature")
	s_str_sig, err := reader.ReadString('\n')
	s_signature := big.NewInt(0)
	s_signature.SetString(s_str_sig, 16)
	fmt.Println("Please enter text to verify")
	ver_string, err := reader.ReadString('\n')
	ver_string = strings.TrimSuffix(ver_string, "\n")
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Verification result:", cryptosystem.Verify([]byte(ver_string), s_signature, s_pub))
}
