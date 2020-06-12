package cryptosystem

import (
	"crypto/rand"
	"log"
	"math/big"
)

func GenerateKeyPair(n_bits int) (priv [3]*big.Int, pub [2]*big.Int) {
	var n_bytes int = n_bits / 8
	if n_bits%8 != 0 {
		n_bytes++
	}

	p := BlumsPrime(n_bytes / 2)
	q := BlumsPrime(n_bytes / 2)
	n := big.NewInt(0).Mul(p, q)
	b, _ := rand.Int(rand.Reader, n)

	priv[0] = p
	priv[1] = q
	priv[2] = b
	pub[0] = n
	pub[1] = b

	return priv, pub
}

func Encrypt(open_text []byte, pub [2]*big.Int) (cipher []byte, c1, c2 int) {
	formatted_text := FormatMessage(open_text, pub[0])
	cryptogram := big.NewInt(0)
	x := big.NewInt(0).SetBytes(formatted_text)
	cryptogram.Mul(x, big.NewInt(0).Add(x, pub[1]))
	cryptogram.Mod(cryptogram, pub[0])

	two_inversed := big.NewInt(0).ModInverse(big.NewInt(2), pub[0])

	c1_big := big.NewInt(0).Add(x, big.NewInt(0).Mul(pub[1], two_inversed))
	c1_big.Mod(c1_big, pub[0])
	c1_big.Mod(c1_big, big.NewInt(2))
	c1 = int(c1_big.Int64())

	c2 = big.Jacobi(big.NewInt(0).Add(x, big.NewInt(0).Mul(pub[1], two_inversed)), pub[0])
	if c2 != 1 {
		c2 = 0
	}
	return cryptogram.Bytes(), c1, c2
}

func Decrypt(cipher []byte, c1, c2 int, priv [3]*big.Int) (open_text []byte) {
	cryptogram := big.NewInt(0).SetBytes(cipher)
	n := big.NewInt(0).Mul(priv[0], priv[1])
	b2 := big.NewInt(0).Exp(priv[2], big.NewInt(2), n)
	four_inversed := big.NewInt(0).ModInverse(big.NewInt(4), n)
	roots, err := BlumSqrt(big.NewInt(0).Add(cryptogram, big.NewInt(0).Mul(b2, four_inversed)), priv[0], priv[1])
	if err != nil {
		log.Fatal(err)
	}

	for _, root := range roots {
		two_inversed := big.NewInt(0).ModInverse(big.NewInt(2), n)
		minus_two_inversed := big.NewInt(0).Sub(n, two_inversed)
		decryptogram := big.NewInt(0).Add(big.NewInt(0).Mul(priv[2], minus_two_inversed), root)
		decryptogram.Mod(decryptogram, n)

		check_c1_big := big.NewInt(0).Add(decryptogram, big.NewInt(0).Mul(priv[2], two_inversed))
		check_c1_big.Mod(check_c1_big, n)
		check_c1_big.Mod(check_c1_big, big.NewInt(2))
		check_c1 := int(check_c1_big.Int64())

		check_c2 := big.Jacobi(big.NewInt(0).Add(decryptogram, big.NewInt(0).Mul(priv[2], two_inversed)), n)
		if check_c2 != 1 {
			check_c2 = 0
		}

		if c1 == check_c1 && c2 == check_c2 {
			open_text = Extract(decryptogram.Bytes())
			break
		}
	}
	return open_text
}

func Sign(open_text []byte, priv [3]*big.Int) (sigmature *big.Int) {
	n := big.NewInt(0).Mul(priv[0], priv[1])
	x := FormatMessage(open_text, n)
	for big.Jacobi(big.NewInt(0).SetBytes(x), n) != 1 {
		x = FormatMessage(open_text, n)
	}

	roots, err := BlumSqrt(big.NewInt(0).SetBytes(x), priv[0], priv[1])
	if err != nil {
		log.Fatal(err)
	}
	return roots[0]
}

func Verify(open_text []byte, signature *big.Int, pub [2]*big.Int) (result bool) {
	unverified_formatted := big.NewInt(0).Exp(signature, big.NewInt(2), pub[0])
	unverified := big.NewInt(0).SetBytes(Extract(unverified_formatted.Bytes()))
	if string(unverified.Bytes()) == string(open_text) {
		return true
	} else {
		return false
	}
}
