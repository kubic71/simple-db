#!/bin/bash

# set current working dir to the location if this script, so that it can be invoked from anywhere
cd `dirname $0`

simple_db="../../obj/simple-db"
echo "Running integration tests"
echo "Working dir: " `pwd`

tests=$( ls *.in | sed 's/.in$//' )

run_test () {
    echo "Running test $1..."
    killall simple-db
    echo "executing pc"
    $simple_db pc >/dev/null 2>&1 &

    echo "executing tm"
    $simple_db tm >/dev/null 2>&1 & 
    #$simple_db tm &

    echo -e "\nInput:" 
    cat $1.in
 
    # netcat doesn't exit when reaching the end of input (because there might still be some data coming)
    # therefore we kill it after timeout
    cat $1.in | nc 127.0.0.1 8080 > $1.tmp &
   
    pid=$!
    sleep  0.01
    kill $pid

    echo -e "\nResult:" 
    cat $1.tmp
    echo -e "\n\n"
 
    # TODO Compare actual query results in <testname>.tmp with expected result in <testname>.out  
}

# if test-name was given, execute only this test
# example usage: 
#   $ ./run_tests.sh t01

if [ ! -z $1 ]
then
    tests=$1
fi

for test in $tests
do
    run_test $test
done


