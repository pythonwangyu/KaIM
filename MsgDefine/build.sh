#!/bin/bash
SRC_DIR=./
DST_DIR=./
#C++
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/*.proto
