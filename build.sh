#!/bin/bash
mkdir build
cd build
cmake ..
cmake --build .
mkdir ../bin
cp server ../bin/server
chmod +x ../bin/server