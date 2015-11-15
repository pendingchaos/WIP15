#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import struct
import glxml

def val_to_str(val, group, gl):
    if len(group) == 0:
        return str(val)
    else:
        group = gl.groups[group]
        
        result = None
        
        for k, v in gl.enumValues.iteritems():
            if v == val and k in group.enumNames:
                result = k
                break
        
        if result == None:
            results = []
            
            for k, v in gl.enumValues.iteritems():
                if v == val:
                    results.append(k)
            
            return "InvalidEnum(%s)" % (results)
        else:
            return result

def val(trace, ret):
    type = struct.unpack("B", trace.read(1))[0]
    
    if type == 0:
        return None
    elif type == 1:
        return str(struct.unpack("<%dL" % (struct.unpack("<L", trace.read(4))[0])))
    elif type == 2:
        return "'%s'" % (trace.read(struct.unpack("<L", trace.read(4))[0]))
    elif type == 3:
        return "0x%x" % (struct.unpack("<L", trace.read(4))[0])
    elif type == 4:
        return "0x%x" % (struct.unpack("<Q", trace.read(8))[0])
    elif type == 5:
        return "<function pointer>"
    elif type == 6:
        value = struct.unpack("<B", trace.read(1))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 7:
        value = struct.unpack("<b", trace.read(1))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 8:
        value = struct.unpack("<H", trace.read(2))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 9:
        value = struct.unpack("<h", trace.read(2))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 10:
        value = struct.unpack("<L", trace.read(4))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 11:
        value = struct.unpack("<l", trace.read(4))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 12:
        value = struct.unpack("<Q", trace.read(8))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 13:
        value = struct.unpack("<q", trace.read(8))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 14:
        value = struct.unpack("<B", trace.read(1))[0] != 0
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 15:
        value = struct.unpack("<L", trace.read(4))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 16:
        value = struct.unpack("<f", trace.read(4))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 17:
        value = struct.unpack("<d", trace.read(8))[0]
        group = trace.read(struct.unpack("<L", trace.read(4))[0])
        return val_to_str(value, group, gl)
    elif type == 18:
        ret[0] = val(trace, ret)
    elif type == 19:
        count = struct.unpack("<L", trace.read(4))[0]
        vals = []
        for i in xrange(count):
            vals.append("'"+trace.read(struct.unpack("<L", trace.read(4))[0])+"'")
        return "["+", ".join(vals)+"]"
    else:
        print "Error reading trace file."

if len(sys.argv) < 2:
    print "Expected two arguments. Got one."
    sys.exit(1)

trace = open(sys.argv[1], "rb")

function_count = struct.unpack("<L", trace.read(4))[0]
id_to_func_name = {}

for i in xrange(function_count):
    id_to_func_name[i] = trace.read(struct.unpack("<L", trace.read(4))[0])

gl = glxml.GL(False)

while True:
    funcID = trace.read(4)
    if len(funcID) == 0:
        break
    funcID = struct.unpack("<L", funcID)[0]
    name = id_to_func_name[funcID]
    
    args = []
    ret = None
    
    while True:
        ret_ = [None]
        arg = val(trace, ret_)
        
        if ret_[0] != None:
            ret = ret_[0]
        elif arg == None:
            break
        else:
            args.append(arg)
    
    if ret != None:
        print "%s(%s) = %s" % (name, ", ".join(args), ret)
    else:
        print "%s(%s)" % (name, ", ".join(args))
