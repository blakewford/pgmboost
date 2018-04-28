#!/bin/bash
for filename in ~/Desktop/training_test_data/training_faces/*; do
    if [ "$filename" != "/home/blakewford/Desktop/training_test_data/training_faces/Thumbs.db" ]; then
        ./adaboost "$filename" >> output.txt
    fi
done
paste -s -d+ output.txt | bc
rm output.txt
