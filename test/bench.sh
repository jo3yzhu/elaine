#!/bin/bash

echo_content=1111111111111111
start=`date +%s`
for (( c=1; c<=100000; c++ ))
do  
    echo_result=`echo -n ${echo_content} | nc 127.0.0.1 1234`
    if [ ${echo_content} != ${echo_result} ]
    then
        echo 'fail'
        break
    fi
done
end=`date +%s`

time=`echo $start $end | awk '{print $2-$1}'`
echo $time