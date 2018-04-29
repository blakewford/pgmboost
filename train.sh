#!/bin/bash
thresh=3
path="/home/blakewford/Desktop/training_test_data/training_faces/"
for filename in `ls $path`; do
    if [ "$filename" != "Thumbs.db" ]; then
         value=`./adaboost "$path$filename"`
        value1=`./adaboost1 "$path$filename"`
        value2=`./adaboost2 "$path$filename"`
        value3=`./adaboost3 "$path$filename"`
        value4=`./adaboost4 "$path$filename"`
        detected=$(echo $value$value1$value2$value3$value4 | bc)
        if [ "$detected" -ge "$thresh" ]; then
            echo "1" >> output.txt
        fi
    fi
done
paste -s -d+ output.txt | bc

thresh2=15
path2="/home/blakewford/Desktop/training_test_data/training_nonfaces/"
for filename in `ls $path2`; do
    if [ "$filename" != "Thumbs.db" ]; then
        value5=`./adaboost "$path2$filename"`
        value6=`./adaboost1 "$path2$filename"`
        value7=`./adaboost2 "$path2$filename"`
        value8=`./adaboost3 "$path2$filename"`
        value9=`./adaboost4 "$path2$filename"`
        detected2=$(echo $value5$value6$value7$value8$value9 | bc)
        if [ "$detected2" -ge "$thresh2" ]; then
            echo "1" >> output2.txt
        fi
    fi
done
paste -s -d+ output2.txt | bc
rm output.txt
rm output2.txt
