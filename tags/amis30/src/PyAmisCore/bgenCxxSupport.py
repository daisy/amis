# C++ support for bgen.
# Preliminary, 14-jun-05, Jack.Jansen@cwi.nl

from bgen import *
from scantools import Scanner
import re

class CxxFunctionGenerator(FunctionGenerator):
    """C++ Function generator.
    
    One refinement when compared to the C function generator: we assign the
    function return value in the declaration of the return variable. This allows
    support of functions returning const values"""

    def declarations(self):
        # Don't declare return value just yet
        if self.rv:
            del self.argumentList[0]
        FunctionGenerator.declarations(self)
        if self.rv:
            self.argumentList.insert(0, self.rv)
            
    def getrvforcallit(self):
        if self.rv:
            decl = self.rv.getArgDeclarations()
            if len(decl) != 1:
                raise RuntimeError, "Type %s cannot be used as return value" % self.rv.type
            return "%s = " % decl[0]
        return ""
    
class CxxMethodGenerator(CxxFunctionGenerator):
    """C++ Method generator.
    
    Almost identical to a FunctionGenerator (not a MethodGenerator: that class
    expects that the underlying C object needs to be passed as an argument!)
    but knows that the method can be called through the object pointer.
    """
    
    def __init__(self, returntype, name, *argumentlist, **conditionlist):
        CxxFunctionGenerator.__init__(self, returntype, name, *argumentlist, **conditionlist)
        
    def setselftype(self, selftype, itselftype):
        CxxFunctionGenerator.setselftype(self, selftype, itselftype)
        if itselftype[-1] == '*':
            self.callname = "_self->ob_itself->" + self.name
        else:
            # Assume non-pointer type
            self.callname = "_self->ob_itself." + self.name
            
class CxxConstructorGenerator(CxxFunctionGenerator):
    """Specialized Method: constructor.
    
    For these objects generate() and such will not be called, in stead
    outputConstructorBody() will output the code fragment needed in the Python
    tp_init function."""

    def setselftype(self, selftype, itselftype):
        CxxFunctionGenerator.setselftype(self, selftype, itselftype)
        if itselftype[-1] == '*':
            self.callname = "new %s" % itselftype[:-1]
        else:
            self.callname = itselftype
        
    
    def generate(self):
        raise RuntimeError, "Cannot call generate() on constructor %s::%s"%(self.itselftype, self.name)
        
    def outputConstructorBody(self):
        OutLbrace()
        self.declarations()
        sep = ",\n" + ' '*len("if (PyArg_ParseTuple(")
        fmt, lst = self.getargsFormatArgs(sep)
        Output("if (PyArg_ParseTuple(_args, \"%s\"%s))", fmt, lst)
        OutLbrace()
        for arg in self.argumentList:
            if arg.flags == SelfMode:
                continue
            if arg.mode in (InMode, InOutMode):
                arg.getargsCheck()
        self.callit()
        Output("return 0;")
        OutRbrace()
        OutRbrace()
        Output()

    def getrvforcallit(self):
        return "((%s *)_self)->ob_itself = " % self.objecttype
        
class CxxGeneratorGroupMixin:
    """Mixin class for ObjectDefinition and Module that handles duplicate
    names."""
    
    def resolveduplicates(self):
        names = {}
        dupnames = {}
        # First collect duplicate names in dupnames, and
        # also initialize the counter to 1
        for g in self.generators:
            name = g.name
            if name in names:
                dupnames[name] = 1
                print 'DBG: duplicate %s.%s' % (self.name, name)
            names[name] = True
        for g in self.generators:
            name = g.name
            if name in dupnames:
                count = dupnames[name]
                dupnames[name] = count + 1
                name = '%s_%d' % (name, count)
                g.name = name
        for g in self.generators:
            if isinstance(g, CxxGeneratorGroupMixin):
                g.resolveduplicates()
        
                
class CxxMixin(CxxGeneratorGroupMixin):
    """Mixin for ObjectDefinition.
    
    The C++ compiler is more picky on use preceding declaration so
    we need a workaround for that.
    """
    
    def outputCheck(self):
        Output("extern PyTypeObject %s;", self.typename)
        Output()
        Output("inline bool %s_Check(PyObject *x)", self.prefix)
        OutLbrace()
##        DedentLevel()
##        Output("#ifdef BGEN_BACK_SUPPORT_%s", self.name)
##        IndentLevel()
        #
        # This version will only accept the exact object type, or
        # C++ subclasses of it.
        # XXXX The "C++ subclasses" bit is not true yet.
        # Objects with Python subtypes as their class will be wrapped
        # in the C++ to Python wrapper, so that added functionality
        # is exported to C++.
        subtypes = ""
##        for stn in self.subtypenames:
##            subtypes += "|| (x)->ob_type == &%s" % stn
        Output("return ((x)->ob_type == &%s%s);", self.typename, subtypes)
##        DedentLevel()
##        Output("#else")
##        IndentLevel()
##        # This version allows subclasses of the given type to
##        # be passed. But note that the object passed to C++ will be the
##        # one with the baseclass type, i.e. without any modified methods.
##        Output("return ((x)->ob_type == &%s || PyObject_TypeCheck((x), &%s));",
##               self.typename, self.typename)
##        DedentLevel()
##        Output("#endif")
##        IndentLevel()
        OutRbrace()
        Output()

    def outputCheckNewArg(self):
        """This implementation assumes we are generating a two-way bridge, and
        the Convert method has been declared a friend of the C++ encapsulation
        class. And it assumes RTTI."""
        # XXX Not sure this is actually correct. What will happen to classes
        # that have been subtyped in C++?? Maybe we should use typeid()?
        DedentLevel()
        Output("#ifdef BGEN_BACK_SUPPORT_%s", self.name)
        IndentLevel()
        Output("%s *encaps_itself = dynamic_cast<%s *>(itself);",
            self.name, self.name)
        Output("if (encaps_itself && encaps_itself->py_%s)", self.name)
        OutLbrace()
        Output("Py_INCREF(encaps_itself->py_%s);", self.name)
        Output("return encaps_itself->py_%s;", self.name)
        OutRbrace()
        DedentLevel()
        Output("#endif")
        IndentLevel()

    def outputCheckConvertArg(self):
        DedentLevel()
        Output("#ifdef BGEN_BACK_SUPPORT_%s", self.name)
        IndentLevel()
        Output("if (!%s_Check(v))", self.prefix)
        OutLbrace()
        Output("*p_itself = Py_WrapAs_%s(v);", self.name)
        Output("if (*p_itself) return 1;")
        OutRbrace()
        DedentLevel()
        Output("#endif")
        IndentLevel()

    def outputTypeObjectInitializerCompat(self):
        pass
        
class CxxModule(CxxGeneratorGroupMixin, Module):
    pass
    
class CxxScanner(Scanner):
    """Pretty dumb C++ scanner.
    
    It is basically the standard Scanner, but it caters for namespace and
    class declarations. It also has rudimental understanding of default
    arguments and inline code (it tries to skip it).
    
    Methods must be declared virtual in C++ to be seen,
    functions extern. Change self.head_pat and type_pat to fix this.
    Unqualified class name is used as the Python class name, and methods are
    for class xyzzy are stored in methods_xyzzy by the generated script.
    """
    def __init__(self, input=None, output=None, defsoutput=None):
        Scanner.__init__(self, input, output, defsoutput)
        self.initnamespaces()
        self.silent = 0
        self.debug = 0
        
    def initnamespaces(self):
        self.namespaces = []
        self.in_class_defn = 0
        self.last_scope_name = None
        self.last_scope_was_class = False
        self.visible = "public"

    def pythonizename(self, name):        
        if '<' in name or '>' in name:
            self.error("Use of templates in typename or functionname not supported: %s", name)
        name = re.sub("\*", " ptr", name)
        name = re.sub("&", " ref", name)
        name = re.sub("::", " ", name)
        name = name.strip()
        name = re.sub("[ \t]+", "_", name)
        return name
        
    def modifyarg(self, type, name, mode):
        if type[:6] == "const_":
            type = type[6:]
            mode = mode + "+ConstMode"
            # Note that const ref and const ptr parameters stay InMode
            if type[-4:] == '_ref':
                type = type[:-4]
                mode = mode + "+RefMode"
        elif type[-4:] == '_ref':
            type = type[:-4]
            mode = "OutMode+RefMode"
        elif type in self.inherentpointertypes:
            mode = "OutMode"
        if type[-4:] == "_far":
            type = type[:-4]
        return type, name, mode

    def initpatterns(self):
        # Patterns to find function/method declaration outside of classes
        self.head1_pat = r"^\s*(static|virtual|extern|pyapi)\s+"
        self.tail_pat = r"[;={}]"
        self.type1_pat = r"(?P<storage>static|virtual|extern|pyapi)" + \
                        r"\s+" + \
                        r"(?P<type>[a-zA-Z0-9_:\s]*[a-zA-Z0-9_*&\s]+[\s*&]\s*)"
        self.name_pat = r"(?P<name>[a-zA-Z0-9_]+)\s*"
        self.args_pat = r"\((?P<args>[^)]*)\)"
        self.const_pat = r"\s*(?P<const>const)?"
        self.whole1_pat = self.type1_pat + self.name_pat + self.args_pat + self.const_pat
        # Patterns to detect same within class declarations
        self.head2_pat = r"^\s*[a-zA-Z0-9_:\s]*[a-zA-Z0-9_]+[\s*\&]"
        self.type2_pat = r"\s*(?P<storage>static|virtual|extern|pyapi)?" + \
                        r"\s+" + \
                        r"(?P<type>[a-zA-Z0-9_:\s]*[a-zA-Z0-9_*&\s]+[\s*&]\s*)"
        self.whole2_pat = self.type2_pat + self.name_pat + self.args_pat + self.const_pat
        # Default to outside-class:
        self.head_pat = self.head1_pat
        self.type_pat = self.type1_pat
        self.whole_pat = self.whole1_pat
        # More special patterns to find constructors
        self.headconstructor_pat = r"\s*(?P<name>[a-zA-Z0-9_]+)\s*\("
        self.typeconstructor_pat = r"^(?P<const>)(?P<storage>)(?P<type>)\s*"
        self.wholeconstructor_pat = self.typeconstructor_pat + self.name_pat + self.args_pat + r"\s*(:[^;{]*)?"
        
        self.sym_pat = r"^[ \t]*(?P<name>[a-zA-Z0-9_]+)[ \t]*=" + \
                       r"[ \t]*(?P<defn>[-0-9_a-zA-Z'\"\(][^\t\n,;}]*),?"
        self.asplit_pat = r"^(?P<type>[^=]*)([^=a-zA-Z0-9_:](?P<name>[a-zA-Z0-9_]+))?(?P<array>\[\])?(?P<initializer>\s*=[a-zA-Z0-9_ ]+)?$"
        self.comment1_pat = r"(?P<rest>.*)//.*"
        # note that the next pattern only removes comments that are wholly within one line
        self.comment2_pat = r"(?P<rest1>.*)/\*.*\*/(?P<rest2>.*)"
        self.namespace_pat = r"^\s*namespace\s+(?P<name>[a-zA-Z0-9_:]+)\s+"
        self.klass_pat = r"^\s*class\s+(AMBULANTAPI\s+)?(?P<name>[a-zA-Z0-9_:]+)\s+"
        self.visibility_pat = r"^\s*(?P<visibility>protected|private|public)\s*:\s*$"

    def donamespace(self, match):
        if self.in_class_defn:
            self.report("Cannot do namespace inside class")
        name = match.group("name")
        self.last_scope_name = name
        self.last_scope_was_class = False
        if self.debug:
            self.report("      %d: namespace %s" % (len(self.namespaces), name))
        
    def doclass(self, match):
        name = match.group("name")
        self.last_scope_name = name
        self.last_scope_was_class = True
        self.visible = "private"
        if self.debug:
            self.report("      %d: namespace %s" % (len(self.namespaces), name))
            
    def dobeginscope(self, count):
        for i in range(count):
            if self.last_scope_name:
                self.namespaces.append(self.last_scope_name)
                self.last_scope_name = None
                if self.last_scope_was_class:
                    self.in_class_defn += 1
            else:
                self.namespaces.append("<scope>")
                if self.in_class_defn:
                    self.in_class_defn += 1
        if self.in_class_defn:
            self.head = self.head2
            self.type = self.type2
            self.whole = self.whole2
        
    def doendscope(self, count):
        for i in range(count):
            if self.in_class_defn:
                self.in_class_defn -= 1
                assert self.in_class_defn >= 0
            if self.debug:
                self.report("      %d: leaving %s" % (len(self.namespaces), self.namespaces[-1]))
            del self.namespaces[-1]
            count -= 1
        if not self.in_class_defn:
            self.head = self.head1
            self.type = self.type1
            self.whole = self.whole1
            
    def dovisibility(self, match):
        ## assert self.in_class_defn
        self.visible = match.group("visibility")
        if self.debug:
            self.report("      %d: %s:" % (len(self.namespaces), self.visible))

    def scan(self):
        if not self.scanfile:
            self.error("No input file has been specified")
            return
        inputname = self.scanfile.name
        self.report("scanfile = %r", inputname)
        if not self.specfile:
            self.report("(No interface specifications will be written)")
        else:
            self.report("specfile = %r", self.specfile.name)
            self.specfile.write("# Generated from %r\n\n" % (inputname,))
        if not self.defsfile:
            self.report("(No symbol definitions will be written)")
        else:
            self.report("defsfile = %r", (self.defsfile.name,))
            self.defsfile.write("# Generated from %r\n\n" % (os.path.split(inputname)[1],))
            self.writeinitialdefs()
        self.alreadydone = []
        try:
            while 1:
                try: line = self.getline()
                except EOFError: break
                if self.debug:
                    self.report("LINE: %r" % (line,))

                match = self.comment1.match(line)
                if match:
                    line = match.group('rest')
                    if self.debug:
                        self.report("\tafter comment1: %r" % (line,))

                match = self.comment2.match(line)
                while match:
                    line = match.group('rest1')+match.group('rest2')
                    if self.debug:
                        self.report("\tafter comment2: %r" % (line,))
                    match = self.comment2.match(line)

                if self.defsfile:
                    match = self.sym.match(line)
                    if match:
                        if self.debug:
                            self.report("\tmatches sym.")
                        self.dosymdef(match)
                        continue

                match = self.head.match(line)
                if match:
                    if self.debug:
                        self.report("\tmatches head.")
                    line = self.dofuncspec()
                elif self.in_class_defn:
                    match = self.headconstructor.match(line)
                    if match and match.group('name') == self.namespaces[-1]:
                        if self.debug:
                            self.report("\tmatches constructor head.")
                        # It appears to be a constructor. Temporarily use
                        # different patterns for type and whole, which will
                        # keep dofuncspec() somewhat happy.
                        keep_type = self.type
                        keep_whole = self.whole
                        self.type = self.typeconstructor
                        self.whole = self.wholeconstructor
                        line = self.dofuncspec()
                        self.type = keep_type
                        self.whole = keep_whole
                            
                match = self.namespace.match(line)
                if match:
                    if self.debug:
                        self.report("\tmatches namespace.")
                    self.donamespace(match)

                match = self.klass.match(line)
                if match:
                    if self.debug:
                        self.report("\tmatches class.")
                    self.doclass(match)

                match = self.visibility.match(line)
                if match:
                    self.dovisibility(match)
                    
                beginscopecount = line.count('{')
                endscopecount = line.count('}')
                if beginscopecount > endscopecount:
                    self.dobeginscope(beginscopecount-endscopecount)
                elif beginscopecount < endscopecount:
                    self.doendscope(endscopecount-beginscopecount)
        except EOFError:
            self.error("Uncaught EOF error")
        self.reportusedtypes()

    def getmodifiers(self, match):
        modifiers = []

        if match.group('storage') == 'static':
            modifiers.append('static')
        elif match.group('storage') == 'virtual':
            modifiers.append('virtual')
            
        if match.group('const') == 'const':
            modifiers.append('const')

        return modifiers
        
    def checkduplicate(self, name):
        """By default we do not check for duplicates in C++ code"""
        self.alreadydone.append(name)
        return False

    def destination(self, type, name, arglist, modifiers=[]):
        if self.in_class_defn:
            classname = self.namespaces[-1]
            classname = self.pythonizename(classname)
            # First check that we don't skip this class altogether
            if classname in self.blacklisttypes:
                self.report("*** class %s blacklisted", classname)
                return None, None
            
            #Then, skip non-public methods
            if self.visible != 'public':
                self.report("*** %s method skipped", self.visible)
                return None, None
                
            # Next, treat static methods as functions.
            if "static" in modifiers:
                return "Function", "functions"
            
            # Also treat constructors as functions, but append to the method list
            if name == self.namespaces[-1]:
                return "ConstructorMethod", "methods_%s" % classname
                
            # Finally treat const methods differently
            if "const" in modifiers:
                return "ConstMethod", "methods_%s" % classname
            return "Method", "methods_%s" % classname
        return "Function", "functions"

    def generatemodifiers(self, classname, name, modifiers):
        # If we are using namespaces make sure to fully qualify
        # all functions.
        if classname == 'Function' and self.namespaces:
            callname = '::'.join(self.namespaces + [name])
            self.specfile.write('    callname="%s",\n' % callname)
        # If this is a constructor we call it with "new"
        if self.in_class_defn and classname == 'Method' and \
                self.namespaces and self.namespaces[-1] == name:
            callname = 'new ' + '::'.join(self.namespaces + [name])
            self.specfile.write('    callname="%s",\n' % callname)
        if modifiers:
            self.specfile.write('    modifiers=%s,\n' % repr(modifiers))
            
    def repairarglist(self, functionname, arglist):
        # More trickery for constructors: if we're in a constructor
        # modify the return arg to return a pointer to the class name
        if self.in_class_defn \
                and arglist == [('', self.namespaces[-1], 'ReturnMode')]:
            return [(self.namespaces[-1]+'_ptr', self.namespaces[-1], 'ReturnMode')]
        return Scanner.repairarglist(self, functionname, arglist)
        

class TupleType(Type):

    def __init__(self, type, *memberlist):
        self.typeName = type
        self.memberlist = memberlist
        self.dot = '.'
        
    def getargsPreCheck(self, name):
        for membertype, membername in self.memberlist:
            membertype.getargsPreCheck(name + self.dot + membername)
        
    def getargsFormat(self):
        fmt = '('
        for membertype, membername in self.memberlist:
            fmt += membertype.getargsFormat()
        fmt += ')'
        return fmt
        
    def getargsArgs(self, name):
        alist = []
        for membertype, membername in self.memberlist:
            arg = membertype.getargsArgs(name + self.dot + membername)
            if arg:
                alist.append(arg)
        return ', '.join(alist)
          
    def getargsCheck(self, name):
        for membertype, membername in self.memberlist:
            membertype.getargsCheck(name + self.dot + membername)
        
    def mkvalueFormat(self):
        fmt = '('
        for membertype, membername in self.memberlist:
            fmt += membertype.mkvalueFormat()
        fmt += ')'
        return fmt
        
    def mkvalueArgs(self, name):
        alist = []
        for membertype, membername in self.memberlist:
            alist.append(membertype.mkvalueArgs(name + self.dot + membername))
        return ', '.join(alist)

class ByAddressTupleType(TupleType):
    def passInput(self, name):
        return "&%s" % name
        
class ByReferenceTupleType(TupleType):

    def passOutput(self, name):
        return name
        
class HeapTupleType(TupleType):

    def __init__(self, type, *memberlist):
        self._realtype = type
        TupleType.__init__(self, type + '*', *memberlist)
        self.dot = '->'

    def getargsPreCheck(self, name):
        # XXX This is a hack: we allocate the object on the heap
        # but never free it!
        Output("%s = new %s;", name, self._realtype)

    def passOutput(self, name):
        return name

class CallbackTupleType(TupleType):

    def __init__(self, type, *memberlist):
        TupleType.__init__(self, type + '*', *memberlist)
        self.dot = '->'
        
    def getargsFormat(self):
        raise RuntimeError, \
            "CallbackTupleType('%s') can only be used for callback input parameters" % self.typeName

    def getargsArgs(self, name):
        raise RuntimeError, \
            "CallbackTupleType('%s') can only be used for callback input parameters" % self.typeName

class StdStringType(Type):
    def __init__(self, typeName="std::string", celement="char", fmt="s"):
        Type.__init__(self, typeName, fmt)
        self.typeName = typeName
        self.celement = celement
        
    def getargsPreCheck(self, name):
        name = name.replace('.', '_')
        Output("%s *%s_cstr;", self.celement, name)
        
    def getargsArgs(self, name):
        name = name.replace('.', '_')
        return "&%s_cstr" % name
        
    def getargsCheck(self, name):
        tmpname = name.replace('.', '_')
        Output("%s = %s_cstr;", name, tmpname)
        
    def mkvalueArgs(self, name):
        return "%s.c_str()" % name

class StdPairType(Type):
    def __init__(self, firsttype, secondtype, typeName="std::pair<%s, %s>", castname=None):
        if '%' in typeName:
            typeName = decl % (firsttype.typeName, secondtype.typeName)
        Type.__init__(self, typeName, "")
        self.firsttype = firsttype
        self.secondtype = secondtype
        if not castname:
            castname = typeName
        self.castname = castname
        
    def getargsPreCheck(self, name):
        decls = self.firsttype.getArgDeclarations(name + "_first") + \
                self.firsttype.getAuxDeclarations(name + "_first") + \
                self.secondtype.getArgDeclarations(name + "_second") + \
                self.secondtype.getAuxDeclarations(name + "_second")
        for d in decls:
            Output("%s;", d)
        self.firsttype.getargsPreCheck(name + "_first")
        self.secondtype.getargsPreCheck(name + "_second")
        
    def getargsArgs(self, name):
        return ', '.join([self.firsttype.getargsArgs(name + "_first"),
               self.secondtype.getargsArgs(name + "_second")])
               
    def getargsFormat(self):
        return '(' + self.firsttype.getargsFormat() + \
               self.secondtype.getargsFormat() + ')'
               
    def getargsCheck(self, name):
        self.firsttype.getargsCheck(name + "_first")
        self.secondtype.getargsCheck(name + "_second")
        Output("%s = %s(%s_first, %s_second);", name, self.castname, name, name)
        
    def mkvalueArgs(self, name):
        return ", ".join([self.firsttype.mkvalueArgs(name + ".first"),
               self.secondtype.mkvalueArgs(name + ".second")])
               
    def mkvalueFormat(self):
        return '(' + self.firsttype.mkvalueFormat() + \
               self.secondtype.mkvalueFormat() + ')'
