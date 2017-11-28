#!/usr/bin/env python
# -*- coding: utf-8 -*-
import subprocess
import tempfile
import os.path
import sys
import os

compiler = sys.argv[1]
input_fname = sys.argv[2]
output_fname = sys.argv[3]
args = sys.argv[4:]

if input_fname.endswith('.replay.c'):
    input_fname = os.path.abspath(input_fname)
    result = tempfile.mktemp('.c')
    
    old_cwd = os.getcwd()
    os.chdir(os.path.split(sys.argv[0])[0])
    subprocess.call(['python', 'preprocess_replay.py', input_fname, result])
    os.chdir(old_cwd)
    
    subprocess.call([compiler, '-c', result, '-o', output_fname] + args)
    
    os.remove(result)
else:
    subprocess.call([compiler, '-c', input_fname, '-o', output_fname] + args)
