#!/bin/sh

script_name=`basename "$0"`
folder="$( cd "$(dirname "$0")" ; pwd -P )"

echo "\n"
# Loops through every file of the test directory.
# If we find some executables, we will assume that
# there are some compiled tests (it is a bit gross yes)!
for file in "$folder/"*; do

    file $file | grep -q "ELF"
    if [ "$?" -eq 0 -a -x "$file" ]; then
        mpirun -n 4 "$file"
        echo "\n"
    fi

done