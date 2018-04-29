#!/bin/bash
thresh=15
values=`./adaboost "$1" 1`;
value=$(echo $values | bc);
if [ "$value" -ge "$thresh" ]; then
    echo "Face detected"
else
    values2=`./adaboost2 "$1" 1`;
    value2=$(echo $values+$values2 | bc);
    if [ "$value2" -ge "$thresh" ]; then
        echo "Face detected"
    else
        values3=`./adaboost4 "$1" 1`;
        value3=$(echo $value2+$values3 | bc);
        if [ "$value3" -ge "$thresh" ]; then
            echo "Face detected"
        else
            values4=`./adaboost1 "$1" 1`;
            value4=$(echo $value3+$values4 | bc);
            if [ "$value4" -ge "$thresh" ]; then
                echo "Face detected"
            else
                values5=`./adaboost3 "$1" 1`;
                value5=$(echo $value4+$values5 | bc);
                if [ "$value5" -ge "$thresh" ]; then
                    echo "Face detected"
                fi
            fi
        fi
    fi
fi
