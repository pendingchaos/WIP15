import xml.etree.cElementTree as ET
import sys
if sys.version_info.major > 2:
    import urllib.request as _urllib_request
    import urllib.error as _urllib_error
    urlopen = _urllib_request.urlopen
    URLError = _urllib_error.URLError
else:
    import urllib2 as _urllib2
    urlopen = _urllib2.urlopen
    URLError = _urllib2.URLError
import os.path
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
        base = os.path.split(__file__)[0]
        
        if download:
            try:
                print('Retreiving gl.xml from the registry.')
                
                url = 'https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/master/xml/gl.xml'
                data = urlopen(url).read()
                
                print('Writing to gl.xml on this computer.')
                
                with open(os.path.join(base, 'gl.xml'), 'w') as glXML:
                    glXML.write(data)
                
                print('Retreiving glx.xml from the registry.')
                
                url = 'https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/master/xml/glx.xml'
                data = urlopen(url).read()
                
                print('Writing to glx.xml on this computer.')
                
                with open(os.path.join(base, 'glx.xml'), 'w') as glXXML:
                    glXXML.write(data)
            except URLError:
                print('Failed to retrieve gl.xml or glx.xml from khronos.org. Using gl.xml and glx.xml file on this computer.')
        
        self.groups = {}
        self.enumValues = {}
        self.functions = {}
        self.versions = {}
        self.extensions = {}
        self.typedecls = ''
        
        tree = ET.parse(os.path.join(base, 'gl.xml'))
        root = tree.getroot()
        self._run(root)
        
        tree = ET.parse(os.path.join(base, 'glx.xml'))
        root = tree.getroot()
        self._run(root)
        
        self._create_wip15_functions()
    
    def _create_wip15_functions(self):
        wip15TestFB = Function('void')
        wip15TestFB.params.append(FunctionParam('const GLchar*', 'name'))
        wip15TestFB.params.append(FunctionParam('const GLvoid*', 'color'))
        wip15TestFB.params.append(FunctionParam('const GLvoid*', 'depth'))
        self.functions['wip15TestFB'] = wip15TestFB
        
        wip15BeginTest = Function('void')
        wip15BeginTest.params.append(FunctionParam('const GLchar*', 'name'))
        self.functions['wip15BeginTest'] = wip15BeginTest
        
        self.functions['wip15EndTest'] = Function('void')
        self.functions['wip15PrintTestResults'] = Function('void')
        
        wip15DrawableSize = Function('void');
        wip15DrawableSize.params.append(FunctionParam('GLsizei', 'width'))
        wip15DrawableSize.params.append(FunctionParam('GLsizei', 'height'))
        self.functions['wip15DrawableSize'] = wip15DrawableSize
        
        wip15ExpectPropertyi64 = Function('void')
        wip15ExpectPropertyi64.params.append(FunctionParam('GLenum', 'objType'))
        wip15ExpectPropertyi64.params.append(FunctionParam('GLuint64', 'objName'))
        wip15ExpectPropertyi64.params.append(FunctionParam('const char*', 'name'))
        wip15ExpectPropertyi64.params.append(FunctionParam('GLuint64', 'index'))
        wip15ExpectPropertyi64.params.append(FunctionParam('GLint64', 'val'))
        self.functions['wip15ExpectPropertyi64'] = wip15ExpectPropertyi64
        
        wip15ExpectPropertyd = Function('void')
        wip15ExpectPropertyd.params.append(FunctionParam('GLenum', 'objType'))
        wip15ExpectPropertyd.params.append(FunctionParam('GLuint64', 'objName'))
        wip15ExpectPropertyd.params.append(FunctionParam('const char*', 'name'))
        wip15ExpectPropertyd.params.append(FunctionParam('GLuint64', 'index'))
        wip15ExpectPropertyd.params.append(FunctionParam('GLdouble', 'val'))
        self.functions['wip15ExpectPropertyd'] = wip15ExpectPropertyd
        
        wip15ExpectPropertybv = Function('void')
        wip15ExpectPropertybv.params.append(FunctionParam('GLenum', 'objType'))
        wip15ExpectPropertybv.params.append(FunctionParam('GLuint64', 'objName'))
        wip15ExpectPropertybv.params.append(FunctionParam('const char*', 'name'))
        wip15ExpectPropertybv.params.append(FunctionParam('GLuint64', 'index'))
        wip15ExpectPropertybv.params.append(FunctionParam('GLuint64', 'size'))
        wip15ExpectPropertybv.params.append(FunctionParam('const void*', 'data'))
        self.functions['wip15ExpectPropertybv'] = wip15ExpectPropertybv
        
        wip15ExpectAttachment = Function('void')
        wip15ExpectAttachment.params.append(FunctionParam('const GLchar*', 'attachment'))
        self.functions['wip15ExpectAttachment'] = wip15ExpectAttachment
    
    def _run(self, root):
        groups = {}
        enumValues = {}
        functions = {}
        versions = {}
        extensions = {}
        
        if root.find('groups') != None:
            for group in root.find('groups').findall('group'):
                newGroup = Group()
                
                for enum in group.findall('enum'):
                    newGroup.enumNames.append(enum.attrib['name'])
                
                groups[group.attrib['name']] = newGroup
        
        for enums_ in root.findall('enums'):
            if 'group' in list(enums_.attrib.keys()):
                if not enums_.attrib['group'] in list(groups.keys()):
                    newGroup = Group()
                    
                    for enum in enums_.findall('enum'):
                        newGroup.enumNames.append(enum.attrib['name'])
                    
                    groups[enums_.attrib['group']] = newGroup
            
            if 'type' in list(enums_.attrib.keys()) and 'group' in list(enums_.attrib.keys()):
                if enums_.attrib['type'] == 'bitmask':
                    groups[enums_.attrib['group']].bitmask = True
            
            for enum in enums_.findall('enum'):
                enumValues[enum.attrib['name']] = eval(enum.attrib['value'])
        
        for group in groups.values():
            toRemove = []
            
            for enumName in group.enumNames:
                if not enumName in enumValues:
                    toRemove.append(enumName)
            
            for toRemove_ in toRemove:
                group.enumNames.remove(toRemove_)
        
        for command in root.find('commands').findall('command'):
            proto = command.find('proto')
            
            rettype = proto.text if proto.text != None else ''
            
            rettype += ''.join([elem.text + elem.tail if elem.tag != 'name' else '' for elem in proto])
            
            func = Function(rettype.lstrip().rstrip())
            command_name = command.find('proto').find('name').text
            
            if command_name in ['glXCreateGLXVideoSourceSGIX', 'glXAssociateDMPbufferSGIX']:
                continue
            
            for param in command.findall('param'):
                ptype = param.find('ptype')
                
                name = param.find('name').text
                type_ = ' '.join([t for t in param.itertext()])
                type_ = ' '.join(type_.split(' ')[:-1])
                type_ = type_.lstrip().rstrip()
                
                if command_name == 'glPathGlyphIndexRangeNV' and name == 'baseAndCount':
                    type_ = 'GLuint[2]'
                
                if type_ == 'const  GLfloat':
                    type_ = 'const GLfloat'
                
                newParam = FunctionParam(type_, name)
                
                if 'group' in param.attrib:
                    if param.attrib['group'] in list(groups.keys()):
                        newParam.group = param.attrib['group']
                
                func.params.append(newParam)
            
            functions[command_name] = func
        
        for feature in root.findall('feature'):
            if feature.attrib['api'] == 'gl':
                version = Version()
                
                number = feature.attrib['number'].split('.')
                
                for require in feature.findall('require'):
                    if require.attrib.get('profile', 'core') != 'core':
                        continue
                    
                    for command in require.findall('command'):
                        version.new_functions.append(command.attrib['name'])
                    
                    for enum in require.findall('enum'):
                        version.new_enums.append(enum.attrib['name'])
                
                for remove in feature.findall('remove'):
                    for command in remove.findall('command'):
                        version.removed_functions.append(command.attrib['name'])
                        
                        # TODO: Some functions were reintroduced in later versions
                        #if (int(number[0]), int(number[1])) == (3, 2):
                        #    del functions[command.attrib['name']]
                    
                    for enum in remove.findall('enum'):
                        version.removed_enums.append(enum.attrib['name'])
                
                versions[(int(number[0]), int(number[1]))] = version
        
        for extension in root.find('extensions').findall('extension'):
            if 'gl' in extension.attrib['supported']:
                newExt = Extension()
                
                for require in extension.findall('require'):
                    if require.attrib.get('profile', 'core') != 'core':
                        continue
                    
                    for command in require.findall('command'):
                        newExt.functions.append(command.attrib['name'])
                
                for enum in require.findall('enum'):
                    newExt.enums.append(enum.attrib['name'])
                
                extensions[extension.attrib['name']] = newExt
        
        for type_ in root.find('types').findall('type'):
            self.typedecls += ' '.join([t for t in type_.itertext()])
            self.typedecls += '\n'
        
        self.groups.update(groups)
        self.enumValues.update(enumValues)
        self.functions.update(functions)
        self.versions.update(versions)
        self.extensions.update(extensions)

if __name__ == '__main__':
    gl = GL(False)

    for name in gl.groups:
        group = gl.groups[name]
        
        print('%s (bitmask=%s):' % (name, 'True' if group.bitmask else 'False'))
        
        for enum in group.enumNames:
            print('    %s = %d' % (enum, gl.enumValues[enum]))

    for name in gl.functions:
        function = gl.functions[name]
        
        params = []
        
        for param in function.params:
            if param.group != None:
                params.append('%s %s' % (param.group, param.name))
            else:
                params.append('%s %s' % (param.type_, param.name))
        
        print('%s %s(%s)' % (function.returnType, name, ', '.join(params)))

    for ver in gl.versions:
        print('%d.%d:' % ver)
        
        version = gl.versions[ver]
        
        for func in version.new_functions:
            print('    +%s' % func)
        
        for enum in version.new_enums:
            print('    +%s' % (enum))
        
        for func in version.removed_functions:
            print('    -%s' % func)
        
        for enum in version.removed_enums:
            print('    -%s' % (enum))

    for ext in gl.extensions:
        print('%s:' % ext)
        
        extension = gl.extensions[ext]
        
        for func in extension.functions:
            print('    +%s' % (func))
        
        for enum in extension.enums:
            print('    +%s' % (enum))
