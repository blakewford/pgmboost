#!/bin/bash
path="/home/blakewford/Desktop/training_test_data/training_faces/"
for filename in `ls $path`; do
    if [ "$filename" != "Thumbs.db" ]; then
        ./adaboost "$path$filename" >> output.txt
    fi
#    sleep 5
done
paste -s -d+ output.txt | bc
path2="/home/blakewford/Desktop/training_test_data/training_nonfaces/"
for filename in `ls $path2`; do
    if [ "$filename" != "Thumbs.db" ]; then
        ./adaboost "$path2$filename" >> output2.txt
    fi
#    sleep 5
done
paste -s -d+ output2.txt | bc
rm output.txt
rm output2.txt
