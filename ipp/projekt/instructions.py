import error
from mytypes import Mytype
import sys


class Instruction:
    labels = {}
    call_stack = []
    data_stack = []
    global_frame = {}
    local_frames = []
    frames = {'GF': global_frame, 'LF': local_frames}
    input = None
    root = None

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
        error.exit(f"invalid {cls.__name__.upper()} structure!", error.Err_codes.xml_bad_structure.value)

    @classmethod
    def arg_isnot_label(cls, arg):
        return arg.attrib["type"] != 'label'

    @classmethod
    def arg_isnot_var(cls, arg):
        return arg.attrib["type"] != 'var'

    @classmethod
    def arg_isnot_const(cls, arg):
        return Mytype.isnot_type(arg.attrib["type"])

    @classmethod
    def arg_isnot_symb(cls, arg):
        return cls.arg_isnot_var(arg) and cls.arg_isnot_const(arg)

    @classmethod
    def var_exists(cls, var):
        # Checks if frame and varieble are defined.
        parts = var.text.split('@')
        try:
            if parts[0] =='LF':
                cls.frames['LF'][-1]
            else:
                cls.frames[parts[0]]
        except:
            error.exit("frame doesn't exist!", error.Err_codes.frame_doesnt_exists.value)
        try:
            if parts[0] =='LF':
                cls.frames['LF'][-1][parts[1]]
            else:
                cls.frames[parts[0]][parts[1]]
        except:
            error.exit("varieble isn't defined!", error.Err_codes.nonexistant_var_used.value)

    @classmethod
    def var_hasvalue(cls, var):
        parts = var.text.split('@')
        if parts[0] =='LF':
            if cls.frames['LF'][-1][parts[1]].name is None or cls.frames['LF'][-1][parts[1]].value is None:
                error.exit("varieble doesn't have a value!", error.Err_codes.missing_value.value)
        else:
            if cls.frames[parts[0]][parts[1]].name is None or cls.frames[parts[0]][parts[1]].value is None:
                error.exit("varieble doesn't have a value!", error.Err_codes.missing_value.value)

    @classmethod
    def var_set(cls, var, name, value):
        # trusts var existence was already checked
        parts = var.text.split('@')
        if parts[0] =='LF':
            cls.frames['LF'][-1][parts[1]] = Mytype(name, value)
        else:
            cls.frames[parts[0]][parts[1]] = Mytype(name, value)

    @classmethod
    def frame_exists(cls, frame):
        if frame == 'TF':
            if 'TF' not in cls.frames:
                error.exit("temp. frame does not exist!", error.Err_codes.frame_doesnt_exists.value)
        elif frame == 'LF':
            if len(cls.frames['LF']) == 0:
                error.exit("local frame does not exist!", error.Err_codes.frame_doesnt_exists.value)

    @classmethod
    def label_exists(cls, label):
        if label not in cls.labels:
            error.exit("label does not exist!", error.Err_codes.semantic_check_err.value)

    @classmethod
    def const_hasvalue(cls, const):
        if const.text == "":
            error.exit("missing constant value!", error.Err_codes.missing_value.value)

    @classmethod
    def symb_exists_and_hasvalue(cls, symb):
        # symb is either const or is var and has to have value
        if cls.arg_isnot_const(symb):
            cls.var_exists(symb)
            cls.var_hasvalue(symb)
        else:
            cls.const_hasvalue(symb)

    @classmethod
    def symb_exists(cls, symb):
        # symb is either const or is var and has to have value
        if cls.arg_isnot_const(symb):
            cls.var_exists(symb)

    @classmethod
    def symb_check_type(cls, symb, type):
        if cls.arg_isnot_const(symb):
            parts = symb.text.split('@')
            if parts[0] == 'LF':
                ok = cls.frames['LF'][-1][parts[1]].is_type(type)
            else:
                ok = cls.frames[parts[0]][parts[1]].is_type(type)
        else:
            ok = symb.attrib["type"] == type
        if not ok:
            error.exit("wrong <symb> types!", error.Err_codes.wrong_operand_types.value)

    @classmethod
    def symb_getvalue(cls, symb):
        if cls.arg_isnot_const(symb):
            parts = symb.text.split('@')
            if parts[0] == 'LF':
                return cls.frames['LF'][-1][parts[1]].value
            else:
                return cls.frames[parts[0]][parts[1]].value
        else:
            tmp = Mytype(symb.attrib["type"], symb.text)
            return tmp.value

    @classmethod
    def symb_getasMytype(cls, symb):
        if cls.arg_isnot_const(symb):
            parts = symb.text.split('@')
            if parts[0] == 'LF':
                return cls.frames['LF'][-1][parts[1]]
            else:
                return cls.frames[parts[0]][parts[1]]
        else:
            return Mytype(symb.attrib["type"], symb.text)

# --------------------------------------------------------------------------------------------------------


class Label(Instruction):
    # LABEL <label>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_label(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        if child[0].text in cls.labels:
            error.exit("redefinition of label!", error.Err_codes.semantic_check_err.value)

    @classmethod
    def exec(cls, child):
        cls.labels[child[0].text] = list(cls.root).index(child)


class Move(Instruction):
    # MOVE <var> <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # var can be empty, symbol not, both have to be defined
        cls.var_exists(child[0])
        if not cls.arg_isnot_var(child[1]):
            cls.var_exists(child[1])
            cls.var_hasvalue(child[1])

    @classmethod
    def exec(cls, child):
        if cls.arg_isnot_var(child[1]):
            # arg2 is const
            name = child[1].attrib["type"]
            value = child[1].text
        else:
            # arg2 is var
            parts = child[1].text.split('@')
            if parts[0] == 'LF':
                name = cls.frames[parts[0]][-1][parts[1]].name
                value = cls.frames[parts[0]][-1][parts[1]].value
            else:
                name = cls.frames[parts[0]][parts[1]].name
                value = cls.frames[parts[0]][parts[1]].value
        cls.var_set(child[0], name, value)


class Createframe(Instruction):
    # delete old TF and create new one

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 0:
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        pass

    @classmethod
    def exec(cls, child):
        if 'TF' in cls.frames:
            del cls.frames['TF']
        cls.frames['TF'] = {}


class Pushframe(Instruction):
    # move TF to the top of LF

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 0:
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # does TF exist?
        cls.frame_exists('TF')

    @classmethod
    def exec(cls, child):
        cls.frames['LF'].append(cls.frames['TF'])
        del cls.frames['TF']


class Popframe(Instruction):
    # move the top of LF to TF

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 0:
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # does LF exist?
        cls.frame_exists('LF')

    @classmethod
    def exec(cls, child):
        cls.frames['TF'] = cls.frames['LF'].pop()


class Defvar(Instruction):
    # create var

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_var(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # frame has to exist
        parts = child[0].text.split('@')
        cls.frame_exists(parts[0])
        # but var cannot
        if parts[0] == 'LF':
            if parts[1] in cls.frames['LF'][-1]:
                error.exit("var redefinition!", error.Err_codes.semantic_check_err.value)
        else:
            if parts[1] in cls.frames[parts[0]]:
                error.exit("var redefinition!", error.Err_codes.semantic_check_err.value)

    @classmethod
    def exec(cls, child):
        parts = child[0].text.split('@')
        if parts[0] == 'LF':
            cls.frames['LF'][-1][parts[1]] = Mytype()
        else:
            cls.frames[parts[0]][parts[1]] = Mytype()


class Call(Instruction):
    # save the PC +1 to call stack and return a PC of given label

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_label(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # label has to exist
        cls.label_exists(child[0].text)

    @classmethod
    def exec(cls, child):
        cls.call_stack.append(int(1 + list(cls.root).index(child)))
        return cls.labels[child[0].text]


class Return(Instruction):
    # pop call stack pc and return it

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 0:
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # call stack cannot be empty
        if len(cls.call_stack) == 0:
            error.exit("call stack is empty, cannot return!", error.Err_codes.missing_value.value)

    @classmethod
    def exec(cls, child):
        return cls.call_stack.pop()


class Pushs(Instruction):
    # PUSHS <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_symb(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # if symb is var -> check it's existence
        if not cls.arg_isnot_var(child[0]):
            cls.var_exists(child[0])
            cls.var_hasvalue(child[0])

    @classmethod
    def exec(cls, child):
        if cls.arg_isnot_var(child[0]):
            name = child[0].attrib["type"]
            value = child[0].text
        else:
            parts = child[0].text.split('@')
            if parts[0] == 'LF':
                name = cls.frames[parts[0]][-1][parts[1]].name
                value = cls.frames[parts[0]][-1][parts[1]].value
            else:
                name = cls.frames[parts[0]][parts[1]].name
                value = cls.frames[parts[0]][parts[1]].value
        cls.data_stack.append(Mytype(name, value))


class Pops(Instruction):
    # POPS <var>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_var(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        # data_stack cannot be empty
        if len(cls.data_stack) == 0:
            error.exit("cannot pop empty stack!", error.Err_codes.missing_value.value)
        # var has to exist
        cls.var_exists(child[0])

    @classmethod
    def exec(cls, child):
        tmp = cls.data_stack.pop()
        cls.var_set(child[0], tmp.name, tmp.value)


class Add(Instruction):
    # ADD <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], "int")
        cls.symb_check_type(child[2], "int")

    @classmethod
    def exec(cls, child):
        res = cls.symb_getvalue(child[1]) + cls.symb_getvalue(child[2])
        cls.var_set(child[0], 'int', res)


class Sub(Add):
    # SUB <var> <symb1> <symb2>

    @classmethod
    def exec(cls, child):
        res = cls.symb_getvalue(child[1]) - cls.symb_getvalue(child[2])
        cls.var_set(child[0], 'int', res)


class Mul(Add):
    # MUL <var> <symb1> <symb2>

    @classmethod
    def exec(cls, child):
        res = cls.symb_getvalue(child[1]) * cls.symb_getvalue(child[2])
        cls.var_set(child[0], 'int', res)


class Idiv(Add):
    # IDIV <var> <symb1> <symb2>
    #  <symb1> // <symb2>

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], "int")
        cls.symb_check_type(child[2], "int")
        if cls.symb_getvalue(child[2]) == 0:
            error.exit("division by zero!", error.Err_codes.bad_operand_value.value)

    @classmethod
    def exec(cls, child):
        res = cls.symb_getvalue(child[1]) // cls.symb_getvalue(child[2])
        cls.var_set(child[0], 'int', res)


class Lt(Instruction):
    # LT <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])

    @classmethod
    def exec(cls, child):
        result = cls.symb_getasMytype(child[1]) < cls.symb_getasMytype(child[2])
        cls.var_set(child[0], 'bool', result)


class Gt(Lt):
    # GT <var> <symb1> <symb2>

    @classmethod
    def exec(cls, child):
        result = cls.symb_getasMytype(child[1]) > cls.symb_getasMytype(child[2])
        cls.var_set(child[0], 'bool', result)


class Eq(Lt):
    # EQ <var> <symb1> <symb2>

    @classmethod
    def exec(cls, child):
        result = cls.symb_getasMytype(child[1]) == cls.symb_getasMytype(child[2])
        cls.var_set(child[0], 'bool', result)


class And(Instruction):
    # AND <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], 'bool')
        cls.symb_check_type(child[2], 'bool')

    @classmethod
    def exec(cls, child):
        result = cls.symb_getasMytype(child[1]).__and__(cls.symb_getasMytype(child[2]))
        cls.var_set(child[0], 'bool', result)


class Or(And):
    # OR <var> <symb1> <symb2>

    @classmethod
    def exec(cls, child):
        # don't know why but this doesn't work with basic 'or'
        result = cls.symb_getasMytype(child[1]).__or__(cls.symb_getasMytype(child[2]))  
        cls.var_set(child[0], 'bool', result)


class Not(Instruction):
    # NOT <var> <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_check_type(child[1], 'bool')

    @classmethod
    def exec(cls, child):
        result = cls.symb_getasMytype(child[1]).__not__()
        cls.var_set(child[0], 'bool', result)


class Int2char(Instruction):
    # INT2CHAR <var> <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_check_type(child[1], 'int')

    @classmethod
    def exec(cls, child):
        try:
            char = chr(cls.symb_getvalue(child[1]))
        except:
            error.exit("non valid Unicode value!", error.Err_codes.invalid_string_operation.value)
        cls.var_set(child[0], 'string', char)


class Stri2int(Instruction):
    # STRI2CHAR <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], 'string')
        cls.symb_check_type(child[2], 'int')

    @classmethod
    def exec(cls, child):
        string = cls.symb_getvalue(child[1])
        try:
            index = cls.symb_getvalue(child[2])
            if index < 0:
                raise IndexError()
            result = ord(string[index])
        except:
            error.exit("non valid Unicode value!", error.Err_codes.invalid_string_operation.value)
        cls.var_set(child[0], 'int', result)


class Read(Instruction):
    # READ <var> <type>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or child[1].attrib['type'] != 'type':
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        if child[1].text not in {'int', 'string', 'bool'}:
            error.exit("given <type> is not correct!", error.Err_codes.bad_operand_value.value)

    @classmethod
    def exec(cls, child):
        try:
            value = cls.input().strip()
        except:
            error.exit("EOF encountered when reading input", error.Err_codes.missing_value.value)
        if value == '':
            cls.var_set(child[0], 'nil', 'nil')
        elif child[1].text == 'bool':
            if value.lower() == 'true':
                cls.var_set(child[0], 'bool', True)
            else:
                cls.var_set(child[0], 'bool', False)
        else:
            try:
                cls.var_set(child[0], child[1].text, value)
            except:
                cls.var_set(child[0], 'nil', 'nil')


class Write(Instruction):
    # WRITE <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_symb(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.symb_exists_and_hasvalue(child[0])

    @classmethod
    def exec(cls, child):
        res = cls.symb_getasMytype(child[0])
        if res.name == 'bool':
            if res.value:
                print("true", end='')
            else:
                print("false", end='')
        elif res.name == 'nil':
            print("", end='')
        else:
            print(res.value, end='')


class Concat(Instruction):
    # CONCAT <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], 'string')
        cls.symb_check_type(child[2], 'string')

    @classmethod
    def exec(cls, child):
        res = cls.symb_getvalue(child[1]) + cls.symb_getvalue(child[2])
        cls.var_set(child[0], 'string', res)


class Strlen(Instruction):
    # STRLEN <var> <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_check_type(child[1], 'string')

    @classmethod
    def exec(cls, child):
        res = len(cls.symb_getvalue(child[1]))
        cls.var_set(child[0], 'int', res)


class Getchar(Instruction):
    # GETCHAR <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], 'string')
        cls.symb_check_type(child[2], 'int')

    @classmethod
    def exec(cls, child):
        index = cls.symb_getvalue(child[2])
        string = cls.symb_getvalue(child[1])
        try:
            if index < 0:
                raise IndexError()
            cls.var_set(child[0], 'string', string[index])
        except:
            error.exit("out of bounds index!", error.Err_codes.invalid_string_operation.value)


class Setchar(Instruction):
    # SETCHAR <var> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.var_hasvalue(child[0])
        cls.symb_check_type(child[0], 'string')

        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])
        cls.symb_check_type(child[1], 'int')
        cls.symb_check_type(child[2], 'string')

    @classmethod
    def exec(cls, child):
        index = cls.symb_getvalue(child[1])
        try:
            char = cls.symb_getvalue(child[2])[0]
        except:
            error.exit("empty string!", error.Err_codes.invalid_string_operation.value)
        string = cls.symb_getvalue(child[0])
        if index > len(string) - 1 or index < 0:
            error.exit("out of bounds index!", error.Err_codes.invalid_string_operation.value)
        string = string[:index] + char + string[index + 1:]
        cls.var_set(child[0], 'string', string)


class Type(Instruction):
    # TYPE <var> <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 2 or cls.arg_isnot_var(child[0]) or cls.arg_isnot_symb(child[1]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.var_exists(child[0])
        cls.symb_exists(child[1])

    @classmethod
    def exec(cls, child):
        type_name = cls.symb_getasMytype(child[1]).name
        if type_name is None:
            cls.var_set(child[0], 'string', "")
        else:
            cls.var_set(child[0], 'string', type_name)


class Jump(Instruction):
    # JUMP <label>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_label(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.label_exists(child[0].text)

    @classmethod
    def exec(cls, child):
        return cls.labels[child[0].text]


class Jumpifeq(Instruction):
    # JUMPIFEQ <label> <symb1> <symb2>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 3 or cls.arg_isnot_label(child[0]) or cls.arg_isnot_symb(child[1]) or cls.arg_isnot_symb(child[2]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.label_exists(child[0].text)
        cls.symb_exists_and_hasvalue(child[1])
        cls.symb_exists_and_hasvalue(child[2])

    @classmethod
    def exec(cls, child):
        if cls.symb_getasMytype(child[1]) == cls.symb_getasMytype(child[2]):
            return cls.labels[child[0].text]


class Jumpifneq(Jumpifeq):
    # JUMPIFNEQ <label> <symb1> <symb2>

    @classmethod
    def exec(cls, child):
        if cls.symb_getasMytype(child[1]).__ne__(cls.symb_getasMytype(child[2])):
            return cls.labels[child[0].text]


class Exit(Instruction):
    # EXIT <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_symb(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.symb_exists_and_hasvalue(child[0])
        cls.symb_check_type(child[0], 'int')

    @classmethod
    def exec(cls, child):
        exit_code = int(cls.symb_getvalue(child[0]))
        if exit_code < 0 or exit_code > 49:
            error.exit("nonvalid exit value!", error.Err_codes.bad_operand_value.value)
        sys.exit(exit_code)


class Dprint(Instruction):
    # DPRINT <symb>

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 1 or cls.arg_isnot_symb(child[0]):
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        cls.symb_exists_and_hasvalue(child[0])

    @classmethod
    def exec(cls, child):
        print(cls.symb_getvalue(child[0]), file=sys.stderr)


class Break(Instruction):
    # BREAK

    @classmethod
    def check_structure(cls, child):
        if cls.arg_count(child) != 0:
            cls.bad_xml()

    @classmethod
    def check_sem(cls, child):
        pass

    @classmethod
    def exec(cls, child):
        print("Printing interpret state info:", file=sys.stderr)
        print("\tCurrently at instruction: " + str(list(cls.root).index(child) + 1), file=sys.stderr)
        print("\tGF: " + str(cls.frames['GF']), file=sys.stderr)
        print("\tTF: " + str(cls.frames['TF']), file=sys.stderr)
        print("\tLFs: ", file=sys.stderr)
        for frame in cls.frames['LF']:
            print("\t\t" + str(frame), file=sys.stderr)
