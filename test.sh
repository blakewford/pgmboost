#!/bin/bash
path="/home/blakewford/Desktop/training_test_data/test_nonfaces/"
for filename in `ls $path`; do
    if [ "$filename" != "Thumbs.db" ]; then
         echo $filename
        ./deploy.sh $path$filename
    fi
done
