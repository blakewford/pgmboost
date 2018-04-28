#!/bin/bash
path="/home/blakewford/Desktop/training_test_data/training_faces/"
for filename in `ls $path`; do
    if [ "$filename" != "Thumbs.db" ]; then
        ./adaboost "$path$filename" >> output.txt
    fi
done
#paste -s -d+ output.txt | bc
#rm output.txt
