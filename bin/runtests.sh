#!/usr/bin/sh
WIP15_TEST=1 ./trace limits/this.limits.txt test.trace ./tests
./leakcheck test.trace
rm test.trace
