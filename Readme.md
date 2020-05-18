# BigInt and Exponentiation
## Crpytography laboratory works

This repository contains BigInteger library which can perform logic and arithmetic operations operating with arbitrary sized integers. The size of the integers limits only by your memory. This library is designed especially to perform exponentiation on 32-bit arc processors. 

1. [How to use](#Usage)
2. [How to build](#Build)
3. [Testing](##Testing)
4. [Extra](#Extra)

## Build 

Usage:
`./exponentiation-main -input a ^ b [-radix 2/10/16]`

This command outputs result of exponentiation a to power b in the radix given in 
argument -r. The default radix is 10.

Folder Test contains tests of the library. 

## Usage

The core library does not have any dependencies. But command line interface requires 
boost C++ library. Tests require GNU MP library and OpenSSL library. So to build tests and CLI you need to install in your system theese dependencied. Please contact me in case you have any issues with the deps.

The build process is pretty simple:

`cmake CMakeLists.txt `  
`make`

## Testing

Test directory contains tests for all core library functionality. Tests are just comparison of outputs of BigInt library and GNU MP library.  
There are also one test containing performance measurements. This test involves measurements of exponentiation functions performance of BigInt, GnuMP and OpenSSL libraries.

## Extra 

Doc folder contains document with report and performance measurement data. This document contains results of performance measurements.