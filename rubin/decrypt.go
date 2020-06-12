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
	fmt.Println("Let's play with decryption")

	priv, pub := cryptosystem.GenerateKeyPair(1024)
	fmt.Printf("n: [%X]\n", pub[0].Bytes())
	fmt.Printf("b: [%X]\n", pub[1].Bytes())

	fmt.Println("Please, enter servers cryptogram")
	s_str_crypto, err := reader.ReadString('\n')
	fmt.Println("Please, enter c1")
	var c1, c2 int
	_, err = fmt.Scanf("%d", &c1)
	fmt.Println("Please, enter c1")
	_, err = fmt.Scanf("%d", &c2)
	if err != nil {
		log.Fatal(err)
	}
	s_cryptogram := big.NewInt(0)
	s_cryptogram.SetString(s_str_crypto, 16)
	decrypted := cryptosystem.Decrypt(s_cryptogram.Bytes(), c1, c2, priv)
	fmt.Println(string((decrypted)))
}
