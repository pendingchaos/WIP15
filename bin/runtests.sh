#!/usr/bin/sh
WIP15_TEST=1 ./trace limits/this.limits.txt test.trace ./tests
./testtrace test.trace
rm test.trace
