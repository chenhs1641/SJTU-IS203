#!/bin/bash

#cd test
for filename in *.seal; do
    echo "--------Test using" $filename "--------"
    ./semant $filename 2> tempfile
    ./semant0 $filename 2> tempfile0
    diff tempfile tempfile0
    if [ $? -eq 0 ]; then
        echo "Passed"
    else
        echo NOT passed
    fi
done
rm -f tempfile tempfile0
#cd ..
