import math

test_names = []
for fname in ['../src/testing/tests/buffer.c']:
    with open(fname, 'r') as f:
        lines = [l for l in f.read().split('\n') if l.startswith('BEGIN_TEST(')]
        for l in lines:
            test_names.append(l[11:].split(')')[0])

done_functions = set([n.split('_')[0] for n in test_names])
functions = set()

current_name = ""
with open('nontrivial_func_impls.c', 'r') as f:
    replay_func_impls_str = f.read()
for line in replay_func_impls_str.split("\n"):
    if line.split('//')[0].rstrip().endswith(":") and line.startswith("gl"):
        if len(current_name) != 0:
            functions.add(current_name)
        current_name = line.split('//')[0].rstrip()[:-1]
if current_name != "":
    functions.add(current_name)

total = len(functions)
done = 0
for f in sorted(functions):
    if f in done_functions:
        done += 1
    else:
        print 'TODO:', f

print '%.0f%c functions tested' % (math.floor(done/float(total)*100), '%')
