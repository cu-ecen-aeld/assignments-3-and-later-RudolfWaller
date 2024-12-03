#!/bin/bash

if [ $# != 2 ]; then
	echo "Usage: writer.sh <filename> <string>"
	exit 1
fi

writefile=$1
writestr=$2

#echo "$writefile"
#echo "$writestr"

mkdir -p $(dirname "$writefile")

if ! touch "$writefile" ; then
	echo "Error writing on file $writefile"
	exit 1
fi

echo "$writestr" > "$writefile"

exit 0

