# BigInt and Exponentiation
## Crpytography laboratory works

This repository contains BigInteger library which can perform logic and arithmetic operations operating with arbitrary sized integers. The size of the integers limits only by your memory. This library is designed especially to perform exponentiation on 32-bit arc processors. 

1. [Usage](#Usage)
2. [Build](#Build)
3. [Testing](#Testing)
4. [Extra](#Extra)

## Usage

Usage:
`./exponentiation-main -input a ^ b [-radix 2/10/16]`

This command outputs result of exponentiation a to power b in the radix given in 
argument -r. The default radix is 10.  

### Constaints

While base value can be any lenght possible, **only 32-bit values are permitted as exponent**. Also this library can only hold **non-negative** values (both exponent and base must be positive or 0)

Folder Test contains tests of the library. 

## Build

The core library does not have any dependencies except small [fmt library](https://fmt.dev/latest/index.html). This library used for string output only. But command line interface requires 
[Boost](https://www.boost.org) C++ library. Tests require [GNU MP](https://gmplib.org) library and [OpenSSL library](https://www.openssl.org). So to build tests and CLI you need to install in your system theese dependencied. Please contact me in case you have any issues with the deps. (Email: dehtyaryov.ipt@gmail.com, Telegram: [link](https://t.me/h_t_f_x)

**This library uses C++17**  
So you may need to find the compiler which supports C++17 std. Please refer [this table](https://en.cppreference.com/w/cpp/compiler_support) to find suitable compiler

The build process is pretty simple:

`cmake CMakeLists.txt `  
`make .`

This project seems to be cross platform. Tested on Linux and Windows 64 bit. 

## Testing

Test directory contains tests for all core library functionality. Tests are just comparison of outputs of BigInt library and GNU MP library.  
There are also one test containing performance measurements. This test involves measurements of exponentiation functions performance of BigInt, GnuMP and OpenSSL libraries.

Tested on input data with maximum length 2048 bits. With this value of operated numbers length library showed real-time or neraly-real time (with relatively small time lags) performance. This lib is still under development, so some enchacements are possible.

## Extra 

Doc folder contains document with report and performance measurement data. This document contains results of performance measurements.
