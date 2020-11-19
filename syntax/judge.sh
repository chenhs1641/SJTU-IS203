#!/bin/bash

cd test
for filename in *.seal; do
    echo "--------Test using" $filename "--------"
    ../parser  $filename > tempfile 
    ../parser0  $filename > tempfile0
    diff tempfile tempfile0
    if [ $? -eq 0 ]; then
        echo passed
    else 
        echo NOT passed
    fi
done
rm -f tempfile
rm -f tempfile0
cd ..