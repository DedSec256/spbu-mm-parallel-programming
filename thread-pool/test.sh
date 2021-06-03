#!/bin/bash

gcc -o thread_pool -std=c99 -lpthread list.c main.c thread_pool.c

./thread_pool 40 4 > thread_pool.log

if [ "$(grep -c -e "worker" thread_pool.log)" -eq 4 ]; then
	echo "PASS: right amount of threads"
else
	echo "FAILED: wrong amount of threads"
fi

