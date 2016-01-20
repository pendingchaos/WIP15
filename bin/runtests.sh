#!/usr/bin/sh
WIP15_TEST=1 ./trace limits/this.limits.txt tests.trace ./tests
./testtrace tests.trace
rm tests.trace
