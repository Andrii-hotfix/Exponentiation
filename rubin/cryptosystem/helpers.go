package cryptosystem

import (
	"crypto/rand"
	"errors"
	"fmt"
	"log"
	"math"
	"math/big"
)

// Ferma primality test
func Ferma(prime *big.Int, iterations int) (result bool) {
	if big.NewInt(2).Cmp(prime) == 0 {
		return true
	} else {
		for i := 0; i < iterations; i++ {
			witness, _ := rand.Int(rand.Reader, big.NewInt(0).Set(prime).Sub(prime, big.NewInt(2)))
			witness.Add(witness, big.NewInt(2))
			if big.NewInt(1).Cmp(big.NewInt(0).GCD(nil, nil, witness, prime)) != 0 {
				return false
			}
			if big.NewInt(0).Exp(witness, big.NewInt(0).Sub(prime, big.NewInt(1)), prime).Cmp(big.NewInt(1)) != 0 {
				return false
			}
		}
		return true
	}
}

// Miller-Rabin primapity test
func MillerRubin(prime *big.Int, iterations int) (result bool) {
	if big.NewInt(0).Mod(prime, big.NewInt(2)).Cmp(big.NewInt(0)) == 0 {
		return false
	} else {
		t := big.NewInt(0).Sub(prime, big.NewInt(1))
		s := big.NewInt(0)
		for big.NewInt(0).Mod(t, big.NewInt(2)).Cmp(big.NewInt(0)) == 0 {
			t.Div(t, big.NewInt(2))
			s.Add(s, big.NewInt(1))
		}
		for i := 0; i < iterations; i++ {
			base, _ := rand.Int(rand.Reader, big.NewInt(0).Set(prime).Sub(prime, big.NewInt(2)))
			exp := big.NewInt(0).Exp(base, t, prime)
			if exp.Cmp(big.NewInt(1)) == 0 || exp.Cmp(big.NewInt(0).Set(prime).Sub(prime, big.NewInt(1))) == 0 {
				continue
			} else {
				for j := big.NewInt(0); j.Cmp(big.NewInt(0).Set(s).Sub(s, big.NewInt(1))) < 0; j.Add(j, big.NewInt(1)) {
					exp.Exp(exp, big.NewInt(2), prime)
					if exp.Cmp(big.NewInt(1)) == 0 {
						return false
					} else if exp.Cmp(big.NewInt(0).Set(prime).Sub(prime, big.NewInt(1))) == 0 {
						break
					}
				}
				return false
			}
		}
		return true
	}
}

func SimpleTest(n *big.Int) (result bool) {
	little_primes := [13]int64{2, 7, 11, 19, 23, 31, 43, 47, 59, 67, 71, 79, 83}
	for _, lprime := range little_primes {
		if big.NewInt(0).Div(n, big.NewInt(lprime)).Cmp(big.NewInt(0)) == 0 {
			return false
		}
	}
	return true
}

func BlumsPrime(n_bytes int) *big.Int {
	primeCh := make(chan *big.Int)
	// seek Blum's prime (p = 4k + 3) using 20 threads
	for i := 0; i < 20; i++ {
		go func() {
			max := big.NewInt(0).Exp(big.NewInt(2), big.NewInt(int64(n_bytes*8)), nil)
			prime, _ := rand.Int(rand.Reader, max)
			prime.Mul(prime, big.NewInt(4)).Add(prime, big.NewInt(3))
			for !Ferma(prime, int(math.Log2(float64(n_bytes*8)))) ||
				!MillerRubin(prime, int(math.Log2(float64(n_bytes*8)))) ||
				!SimpleTest(prime) {
				prime.Add(prime, big.NewInt(2))
			}
			primeCh <- prime
		}()
	}
	return <-primeCh
}

func FormatMessage(text []byte, n *big.Int) (formatted_text []byte) {
	if len(text) <= (len(n.Bytes()) - 10) {
		r, _ := rand.Int(rand.Reader, big.NewInt(0).Exp(big.NewInt(2), big.NewInt(64), nil))
		formatted_text = append(formatted_text, 0x00)
		formatted_text = append(formatted_text, 0xFF)
		formatted_text = append(formatted_text, make([]byte, len(n.Bytes())-10-len(text))...)
		formatted_text = append(formatted_text, text...)
		formatted_text = append(formatted_text, r.Bytes()...)
	}
	return formatted_text
}

func Extract(formatted_text []byte) (text []byte) {
	if formatted_text[0] == 0xFF {
		return big.NewInt(0).SetBytes(formatted_text[2 : len(formatted_text)-8]).Bytes()
	} else {
		log.Fatal("bad formatting")
		return text
	}
}

func BlumSqrt(y *big.Int, p *big.Int, q *big.Int) (result [4]*big.Int, err error) {
	n := big.NewInt(0).Mul(p, q)

	if big.Jacobi(y, n) != 1 {
		return result, errors.New("y is quadratic non residue")
	}

	s1 := big.NewInt(0).Exp(y, big.NewInt(0).Div(big.NewInt(0).Add(p, big.NewInt(1)), big.NewInt(4)), p)
	s2 := big.NewInt(0).Exp(y, big.NewInt(0).Div(big.NewInt(0).Add(q, big.NewInt(1)), big.NewInt(4)), q)

	u := big.NewInt(0)
	v := big.NewInt(0)

	z := big.NewInt(0).GCD(u, v, p, q)
	if z.Cmp(big.NewInt(1)) != 0 {
		return result, errors.New("p or/and q is/are not prime")
	}
	fmt.Println("u: ", u)
	fmt.Println("v: ", v)

	result[0] = big.NewInt(0).Add(
		big.NewInt(0).Mul(big.NewInt(0).Mul(u, p), s2),
		big.NewInt(0).Mul(big.NewInt(0).Mul(v, q), s1))
	result[0].Mod(result[0], n)
	result[1] = big.NewInt(0).Sub(n, result[0])
	result[2] = big.NewInt(0).Sub(
		big.NewInt(0).Mul(big.NewInt(0).Mul(u, p), s2),
		big.NewInt(0).Mul(big.NewInt(0).Mul(v, q), s1))
	result[2].Mod(result[2], n)
	result[3] = big.NewInt(0).Sub(n, result[2])
	return result, nil
}
