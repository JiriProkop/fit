import error


class Instruction:
    labels = {}
    global_frame = {}
    local_frames = []
    frames = {'GF': global_frame, 'LF': local_frames}

    @classmethod
    def exec(cls):
        raise NotImplementedError()

    @classmethod
    def check_structure(cls):
        raise NotImplementedError()

    @classmethod
    def check_sem(cls):
        raise NotImplementedError()

    @classmethod
    def arg_count(cls, child):
        return int(len(list(child)))

    @classmethod
    def bad_xml(cls):
        error.exit(f"invalid {cls.__name__.upper()} structure!",
                   error.Err_codes.xml_bad_structure.value)

    @classmethod
    def arg_isnot_label(cls, arg):
        return arg.attrib["type"] != 'label'

    @classmethod
    def arg_isnot_var(cls, arg):
        return arg.attrib["type"] != 'var'

    @classmethod
    def arg_isnot_const(cls, arg):
        return Type.isnot_type(arg.attrib["type"])

    @classmethod
    def arg_isnot_symb(cls, arg):
        return cls.arg_isnot_var(arg) and cls.arg_isnot_const(arg)

    @classmethod
    def var_exists(cls, var):
        # Checks if frame and varieble are defined.
        parts = var.split('@')
        try:
            if parts[0] == LF:
                cls.frames['LF'][0]
            else:
                cls.frames[parts[0]]
        except:
            error.exit("frame doesn't exist!", error.Err_codes.frame_doesnt_exists.value)
        try:
            if parts[0] == LF:
                cls.frames['LF'][0][parts[1]]
            else:
                cls.frames[parts[0]][parts[1]]
        except:
            error.exit("varieble isn't defined!", error.Err_codes.nonexistant_var_used.value)
        
    @classmethod
    def var_hasvalue(cls, var):
        parts = var.split('@')
        if parts[0] == LF:
            if cls.frames['LF'][0][parts[1]] == "":
                error.exit("varieble isn't defined!", error.Err_codes.nonexistant_var_used.value)
        else:
            if cls.frames[parts[0]][parts[1]] == "":
                error.exit("varieble isn't defined!", error.Err_codes.nonexistant_var_used.value)
        
    @classmethod
    def var_set(cls, var, name, value):
        # trusts var existence was already checked
        parts = var.split('@')
        if parts[0] == LF:
            cls.frames['LF'][0][parts[1]].name = name
            cls.frames['LF'][0][parts[1]].value = value
        else:
            cls.frames[parts[0]][parts[1]].name = name
            cls.frames[parts[0]][parts[1]].value = value


class Label(Instruction):
    # LABEL <label>

    @classmethod
    def exec(cls, child, line):
        cls.labels[child[0].text] = line

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_label(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        if child[0].text in cls.labels:
            error.exit("redefinition of label!",
                       error.Err_codes.semantic_check_err.value)


class Move(Instruction):
    # MOVE <var> <symb>

    # symb is either constant or <var>
    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]):
            cls.bad_xml()

    def check_sem(cls, child):
        # var can be empty, symbol not, both have to be defined
        cls.var_exists(child[0].text)
        if not arg_isnot_var(child[1]):
            cls.var_hasvalue(child[1])

    def exec(cls, child):
        if arg_isnot_var(child[1]):
            # arg2 is const
            name = child[1].attrib["type"]
            value = child[1].text
        else:
            # arg2 is var
            parts = child[1].split('@')
            name = cls.frames[parts[0]][parts[1]].name
            value = cls.frames[parts[0]][parts[1]].value
        cls.var_set(child[0], name, value)


class Type:
    types = ('nil', 'bool', 'int', 'string')

    @classmethod
    def isnot_type(cls, type):
        return type not in cls.types

    def __escape_remove__(self, text):
        res = ''
        i = 0
        while i < len(text):
            if text[i] == '\\':
                # 3 for 3 escape digits and 1 cause len doesnt count from 0 but from 1
                if i + 3 + 1 > len(text):
                    print(len(text), i + 4, text[i])
                    error.exit("escape sequnce has to be 3 digits long!",
                               error.Err_codes.invalid_string_operation.value)
                escape = int(text[i + 1] + text[i + 2] + text[i + 3])
                res += str(chr(escape))
                i += 4
            else:
                res += text[i]
                i += 1
        return res

    def __init__(self, name, value):
        if self.isnot_type(name):
            raise ValueError("Invalid type name.")
        if name == "string":
            value = self.__escape_remove__(value)
        elif name == int:
            value = int(value)
        self.name = name
        self.value = value
