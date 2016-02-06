#!/usr/bin/sh
WIP15_TEST=1 ./trace -o tests.trace ./tests
./testtrace tests.trace
rm tests.trace
