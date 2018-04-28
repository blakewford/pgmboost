#!/bin/bash
path="/home/blakewford/Desktop/training_test_data/test_nonfaces/"
for filename in `ls $path`; do
    if [ "$filename" != "Thumbs.db" ]; then
        echo $filename
        ./adaboost "$path$filename" >> output.txt
    fi
#    sleep 5
done
#paste -s -d+ output.txt | bc
#rm output.txt
