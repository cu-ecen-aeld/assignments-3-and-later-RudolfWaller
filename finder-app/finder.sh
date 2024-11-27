#!/bin/bash

if [ $# != 2 ]; then
    echo "Usage: finder.sh <files dir> <search string>"
    exit 1
fi

filesdir=$1
searchstr=$2

if [ ! -d "$filesdir" ]; then
    echo "Directory $filesdir doesn't exist"
    exit 1    
fi

total_files=$(find "$filesdir" -type f | wc -l)
matching_lines=$(find "$filesdir" -type f -exec grep -c "$searchstr" {} +)
total_matching_lines=$(find "$filesdir" -type f -exec grep -c "$searchstr" {} + | awk -F: '{sum += $2} END {print sum}')

#echo "The total files is $total_files and the matching lines is $total_matching_lines"
echo "The number of files are ${total_files} and the number of matching lines are ${total_matching_lines}"
exit 0

