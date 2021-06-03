#!/bin/bash

gcc -o prodcons -std=c99 -lpthread prodcons.c

echo "test 1) prod 1, cons 5"

timeout --signal=INT 5 ./prodcons 1 5 > prodcons_1_5.log
FINISHED="$(grep -c "finished" prodcons_1_5.log)"

if [ $FINISHED -eq 6 ]; then
	echo "1. PASS: right amount of threads"
else
	echo "1. FAILED: wrong amount of threads"
fi

ADD="$(grep -e "ADD" prodcons_1_5.log | cut -d ' ' -f7 | cut -d ',' -f1)"

DEL="$(grep -e "DEL" prodcons_1_5.log | cut -d ' ' -f7 | cut -d ',' -f1)"

LEFT="$(grep -e "LEFT" prodcons_1_5.log | cut -d ' ' -f3 | cut -d ',' -f1)"

while IFS= read -r line; do ADD="$(echo $ADD | grep -v "$line")"; done <<< "$DEL"

while IFS= read -r line; do ADD="$(echo $ADD | grep -v "$line")"; done <<< "$LEFT"

if [ "$ADD" == "" ]; then
	echo "2. PASS: right balance b/w ADD, DELL and LEFT"
else
	echo "2. FAILED: wrong balance b/w ADD, DELL and LEFT"
fi

##############################################################################

echo "test 2) prod 5, cons 1"

timeout --signal=INT 5 ./prodcons 5 1 > prodcons_5_1.log
FINISHED="$(grep -c "finished" prodcons_5_1.log)"

if [ $FINISHED -eq 6 ]; then
	echo "1. PASS: right amount of threads"
else
	echo "1. FAILED: wrong amount of threads"
fi

ADD="$(grep -e "ADD" prodcons_5_1.log | cut -d ' ' -f7 | cut -d ',' -f1)"

DEL="$(grep -e "DEL" prodcons_5_1.log | cut -d ' ' -f7 | cut -d ',' -f1)"

LEFT="$(grep -e "LEFT" prodcons_5_1.log | cut -d ' ' -f3 | cut -d ',' -f1)"

while IFS= read -r line; do ADD="$(echo $ADD | grep -v "$line")"; done <<< "$DEL"

while IFS= read -r line; do ADD="$(echo $ADD | grep -v "$line")"; done <<< "$LEFT"

if [ "$ADD" == "" ]; then
	echo "2. PASS: right balance b/w ADD, DELL and LEFT"
else
	echo "2. FAILED: wrong balance b/w ADD, DELL and LEFT"
fi

##############################################################################

echo "test 3) prod 5, cons 5"

timeout --signal=INT 5 ./prodcons 5 5 > prodcons_5_5.log
FINISHED="$(grep -c "finished" prodcons_5_5.log)"

if [ $FINISHED -eq 10 ]; then
	echo "1. PASS: right amount of threads"
else
	echo "1. FAILED: wrong amount of threads"
fi

ADD="$(grep -e "ADD" prodcons_5_5.log | cut -d ' ' -f7 | cut -d ',' -f1)"

DEL="$(grep -e "DEL" prodcons_5_5.log | cut -d ' ' -f7 | cut -d ',' -f1)"

LEFT="$(grep -e "LEFT" prodcons_5_5.log | cut -d ' ' -f3 | cut -d ',' -f1)"

while IFS= read -r line; do ADD="$(echo $ADD | grep -v "$line")"; done <<< "$DEL"

while IFS= read -r line; do ADD="$(echo $ADD | grep -v "$line")"; done <<< "$LEFT"

if [ "$ADD" == "" ]; then
	echo "2. PASS: right balance b/w ADD, DELL and LEFT"
else
	echo "2. FAILED: wrong balance b/w ADD, DELL and LEFT"
fi
