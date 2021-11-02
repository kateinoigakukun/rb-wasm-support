#!/bin/bash

curl -LO https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/clang+llvm-12.0.0-x86_64-apple-darwin.tar.xz
tar xfz clang+llvm-12.0.0-x86_64-apple-darwin.tar.xz

curl -LO https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-12/wasi-sdk-12.0-macos.tar.gz
tar xfz wasi-sdk-12.0-macos.tar.gz
