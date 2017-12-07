#!/usr/bin/env python
# -*- coding: utf-8 -*-
import math
from glob import glob

test_names = []
for fname in ['../src/testing/tests/buffer.c']:
    with open(fname, 'r') as f:
        lines = [l for l in f.read().split('\n') if l.startswith('BEGIN_TEST(')]
        for l in lines:
            test_names.append(l[11:].split(')')[0])

done_functions = set([n.split('_')[0] for n in test_names])
functions = set()

replay_func_impls_str = '\n\n'.join([open(f).read() for f in glob('../src/libtrace/replay/*.c')])
for line in replay_func_impls_str.split("\n"):
    if line.split('//')[0].rstrip().endswith(":") and not (line.startswith(' ') or line.startswith('\t')):
        functions.add(line.split('//')[0].rstrip()[:-1])

total = len(functions)
done = 0
for f in sorted(functions):
    if f in done_functions:
        done += 1
    else:
        print('%s %s' % ('TODO:', f))

print('%.0f%c functions tested' % (math.floor(done/float(total)*100), '%'))
