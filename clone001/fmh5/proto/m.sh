#!/bin/sh
cd `pwd`
protoc -I=. --cpp_out=. $1.proto
#protoc -I=. --as3_out=. --plugin=protoc-gen-as3="/usr/local/protoc-gen-as3/protoc-gen-as3" $1.proto
#cp -rp $1/ ../as
#rm -rf initializer.as.inc $1
cp $1.proto ../as
