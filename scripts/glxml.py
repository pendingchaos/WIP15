import xml.etree.ElementTree as ET
import urllib2
import copy

class Group:
    def __init__(self, enumNames=[]):
        self.enumNames = copy.copy(enumNames)
        self.bitmask = False

class FunctionParam:
    def __init__(self, type_, name, group=None):
        self.type_ = type_
        self.name = name
        self.group = group

class Function:
    def __init__(self, returnType):
        self.params = []
        self.returnType = returnType

class Version:
    def __init__(self):
        self.new_functions = []
        self.new_enums = []
        self.removed_functions = []
        self.removed_enums = []

class Extension:
    def __init__(self):
        self.functions = []
        self.enums = []

class GL:
    def __init__(self, download):
        if download:
            try:
                print "Retreiving gl.xml from khronos.org."
                
                url = "https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/gl.xml"
                data = urllib2.urlopen(url).read()
                
                print "Writing to gl.xml on this computer."
                
                with open("gl.xml", "w") as glXML:
                    glXML.write(data)
                
                print "Retreiving glx.xml from khronos.org."
                
                url = "https://cvs.khronos.org/svn/repos/ogl/trunk/doc/registry/public/api/glx.xml"
                data = urllib2.urlopen(url).read()
                
                print "Writing to glx.xml on this computer."
                
                with open("glx.xml", "w") as glXXML:
                    glXXML.write(data)
            except urllib2.URLError:
                print "Failed to retrieve gl.xml or glx.xml from khronos.org. Using gl.xml and glx.xml file on this computer."
        
        self.groups = {}
        self.enumValues = {}
        self.functions = {}
        self.versions = {}
        self.extensions = {}
        self.typedecls = ""
        
        tree = ET.parse("gl.xml")
        root = tree.getroot()
        self._run(root)
        
        tree = ET.parse("glx.xml")
        root = tree.getroot()
        self._run(root)
        
        self._create_wip15_extension()
    
    def _create_wip15_extension(self):
        self.functions["glSetContextCapsWIP15"] = Function("void")
        
        glMappedBufferDataWIP15 = Function("void")
        glMappedBufferDataWIP15.params.append(FunctionParam("GLenum", "target"))
        glMappedBufferDataWIP15.params.append(FunctionParam("GLsizei", "size"))
        glMappedBufferDataWIP15.params.append(FunctionParam("const GLvoid*", "data"))
        self.functions["glMappedBufferDataWIP15"] = glMappedBufferDataWIP15
        
        glProgramUniformWIP15 = Function("void")
        glProgramUniformWIP15.params.append(FunctionParam("GLuint", "program"))
        glProgramUniformWIP15.params.append(FunctionParam("const GLchar*", "name"))
        glProgramUniformWIP15.params.append(FunctionParam("GLuint", "location"))
        self.functions["glProgramUniformWIP15"] = glProgramUniformWIP15
        
        glProgramAttribWIP15 = Function("void")
        glProgramAttribWIP15.params.append(FunctionParam("GLuint", "program"))
        glProgramAttribWIP15.params.append(FunctionParam("const GLchar*", "name"))
        glProgramAttribWIP15.params.append(FunctionParam("GLuint", "index"))
        self.functions["glProgramAttribWIP15"] = glProgramAttribWIP15
        
        glTestFBWIP15 = Function("void")
        glTestFBWIP15.params.append(FunctionParam("const GLchar*", "name"))
        glTestFBWIP15.params.append(FunctionParam("const GLvoid*", "color"))
        glTestFBWIP15.params.append(FunctionParam("const GLvoid*", "depth"))
        self.functions["glTestFBWIP15"] = glTestFBWIP15
        
        glCurrentTestWIP15 = Function("void")
        glCurrentTestWIP15.params.append(FunctionParam("const GLchar*", "name"))
        self.functions["glCurrentTestWIP15"] = glCurrentTestWIP15
        
        glDrawableSizeWIP15 = Function("void");
        glDrawableSizeWIP15.params.append(FunctionParam("GLsizei", "width"))
        glDrawableSizeWIP15.params.append(FunctionParam("GLsizei", "height"))
        self.functions["glDrawableSizeWIP15"] = glDrawableSizeWIP15
        
        ext = Extension()
        ext.functions.append("glSetContextCapsWIP15")
        ext.functions.append("glMappedBufferDataWIP15")
        ext.functions.append("glProgramUniformWIP15")
        ext.functions.append("glProgramAttribWIP15")
        ext.functions.append("glTestFBWIP15")
        ext.functions.append("glCurrentTestWIP15")
        ext.functions.append("glDrawableSizeWIP15")
        
        self.extensions["GL_WIP15_debug_internal"] = ext
    
    def _run(self, root):
        groups = {}
        enumValues = {}
        functions = {}
        versions = {}
        extensions = {}
        
        if root.find("groups") != None:
            for group in root.find("groups").findall("group"):
                newGroup = Group()
                
                for enum in group.findall("enum"):
                    newGroup.enumNames.append(enum.attrib["name"])
                
                groups[group.attrib["name"]] = newGroup
        
        for enums_ in root.findall("enums"):
            if "group" in enums_.attrib.keys():
                if not enums_.attrib["group"] in groups.keys():
                    newGroup = Group()
                    
                    for enum in enums_.findall("enum"):
                        newGroup.enumNames.append(enum.attrib["name"])
                    
                    groups[enums_.attrib["group"]] = newGroup
            
            if "type" in enums_.attrib.keys() and "group" in enums_.attrib.keys():
                if enums_.attrib["type"] == "bitmask":
                    groups[enums_.attrib["group"]].bitmask = True
            
            for enum in enums_.findall("enum"):
                enumValues[enum.attrib["name"]] = eval(enum.attrib["value"])
        
        for group in groups.itervalues():
            toRemove = []
            
            for enumName in group.enumNames:
                if not enumName in enumValues:
                    toRemove.append(enumName)
            
            for toRemove_ in toRemove:
                group.enumNames.remove(toRemove_)
        
        for command in root.find("commands").findall("command"):
            proto = command.find("proto")
            
            rettype = proto.text if proto.text != None else ""
            
            rettype += "".join([elem.text + elem.tail if elem.tag != "name" else "" for elem in proto])
            
            func = Function(rettype.lstrip().rstrip())
            command_name = command.find("proto").find("name").text
            
            if command_name in ["glXCreateGLXVideoSourceSGIX", "glXAssociateDMPbufferSGIX"]:
                continue
            
            for param in command.findall("param"):
                ptype = param.find("ptype")
                
                name = param.find("name").text
                type_ = " ".join([t for t in param.itertext()])
                type_ = " ".join(type_.split(" ")[:-1])
                type_ = type_.lstrip().rstrip()
                
                if command_name == "glPathGlyphIndexRangeNV" and name == "baseAndCount":
                    type_ = "GLuint[2]"
                
                if type_ == "const  GLfloat":
                    type_ = "const GLfloat"
                
                newParam = FunctionParam(type_, name)
                
                if "group" in param.attrib:
                    if param.attrib["group"] in groups.keys():
                        newParam.group = param.attrib["group"]
                
                func.params.append(newParam)
            
            functions[command_name] = func
        
        for feature in root.findall("feature"):
            if feature.attrib["api"] == "gl":
                version = Version()
                
                number = feature.attrib["number"].split(".")
                
                for require in feature.findall("require"):
                    for command in require.findall("command"):
                        version.new_functions.append(command.attrib["name"])
                    
                    for enum in require.findall("enum"):
                        version.new_enums.append(enum.attrib["name"])
                
                for remove in feature.findall("remove"):
                    for command in remove.findall("command"):
                        version.removed_functions.append(command.attrib["name"])
                        
                        # TODO: Some functions were reintroduced in later versions
                        if (int(number[0]), int(number[1])) == (3, 2):
                            del functions[command.attrib["name"]]
                    
                    for enum in remove.findall("enum"):
                        version.removed_enums.append(enum.attrib["name"])
                
                versions[(int(number[0]), int(number[1]))] = version
        
        for extension in root.find("extensions").findall("extension"):
            if "gl" in extension.attrib["supported"]:
                newExt = Extension()
                
                for require in extension.findall("require"):
                    for command in require.findall("command"):
                        newExt.functions.append(command.attrib["name"])
                
                for enum in require.findall("enum"):
                    newExt.enums.append(enum.attrib["name"])
                
                extensions[extension.attrib["name"]] = newExt
        
        for type_ in root.find("types").findall("type"):
            self.typedecls += " ".join([t for t in type_.itertext()])
            self.typedecls += "\n"
        
        self.groups.update(groups)
        self.enumValues.update(enumValues)
        self.functions.update(functions)
        self.versions.update(versions)
        self.extensions.update(extensions)

if __name__ == "__main__":
    gl = GL(False)

    for name in gl.groups:
        group = gl.groups[name]
        
        print "%s (bitmask=%s):" % (name, "True" if group.bitmask else "False")
        
        for enum in group.enumNames:
            print "    %s = %d" % (enum, gl.enumValues[enum])

    for name in gl.functions:
        function = gl.functions[name]
        
        params = []
        
        for param in function.params:
            if param.group != None:
                params.append("%s %s" % (param.group, param.name))
            else:
                params.append("%s %s" % (param.type_, param.name))
        
        print "%s %s(%s)" % (function.returnType, name, ", ".join(params))

    for ver in gl.versions:
        print "%d.%d:" % ver
        
        version = gl.versions[ver]
        
        for func in version.new_functions:
            print "    +%s" % func
        
        for enum in version.new_enums:
            print "    +%s" % (enum)
        
        for func in version.removed_functions:
            print "    -%s" % func
        
        for enum in version.removed_enums:
            print "    -%s" % (enum)

    for ext in gl.extensions:
        print "%s:" % ext
        
        extension = gl.extensions[ext]
        
        for func in extension.functions:
            print "    +%s" % (func)
        
        for enum in extension.enums:
            print "    +%s" % (enum)
