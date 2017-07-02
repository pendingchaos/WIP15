#!/usr/bin/sh
./trace -o tests.trace ./tests
./replaytrace tests.trace
rm tests.trace
