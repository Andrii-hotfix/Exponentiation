package main

import (
	"./cryptosystem"
	"bufio"
	"fmt"
	"log"
	"os"
	"strings"
)

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Time to check the signature on the server:")

	priv, pub := cryptosystem.GenerateKeyPair(1024)
	fmt.Printf("n: [%X]\n", pub[0].Bytes())

	fmt.Println("enter a message to sign:")
	sign_msg, err := reader.ReadString('\n')
	sign_msg = strings.TrimSuffix(sign_msg, "\n")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("signature")
	fmt.Printf("[%X]\n", cryptosystem.Sign([]byte(sign_msg), priv).Bytes())
}
