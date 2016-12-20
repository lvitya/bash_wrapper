#!/bin/bash
# run ./test1.sh to test bash_wrapper

echo "echo from test1"
echo $@

PARENT_COMMAND="$(ps -o comm= $PPID)"
echo "PARENT_COMMAND:" $PARENT_COMMAND

read
printf "\nrun test2 from test1\n"
./test2.sh

printf "\nrun test3 from test1\n"
./test3.sh

