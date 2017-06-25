#!/usr/bin/sh
./trace -o tests.trace ./tests
./testtrace tests.trace
rm tests.trace
