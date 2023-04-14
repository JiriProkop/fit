import error

class Mytype:
    names = ('nil', 'bool', 'int', 'string', None)

    @classmethod
    def isnot_type(cls, type):
        return type not in cls.names

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

    def __init__(self, name=None, value=None):
        if self.isnot_type(name):
            raise ValueError("Invalid type name.")
        if name == "string":
            if value is None:
                value = ""
            else:
                value = self.__escape_remove__(str(value))
        elif name == 'int':
            try:
                value = int(value)
            except:
                error.exit("invalid integer value!", error.Err_codes.xml_bad_structure.value)
        elif name == 'bool':
            if value == 'false':
                value = False
            try:
                value = bool(value.capitalize())  # careful! any non empty string which is not 'False' is converted to True
            except:
                pass
        self.name = name
        self.value = value

    def is_type(self, type):
        return self.name == type

    def __eq__(self, other):
        if self.name != other.name:
            error.exit("EQ can be used only with same types!", error.Err_codes.wrong_operand_types.value)
        else:
            return self.value == other.value

    def __ne__(self, other):
        return not self.__eq__(other)

    def __qt__(self, other):
        if self.name != other.name or self.name == 'nil' or other.name == 'nil':
            error.exit("QT can be used only with same types!", error.Err_codes.wrong_operand_types.value)
        else:
            return self.value > other.value

    def __lt__(self, other):
        if self.name != other.name or self.name == 'nil' or other.name == 'nil':
            error.exit("LT can be used only with same types!", error.Err_codes.wrong_operand_types.value)
        else:
            return self.value < other.value

    def __or__(self, other):
        if self.name != 'bool' or other.name != 'bool':
            error.exit("'or' can be used only with bools!", error.Err_codes.wrong_operand_types.value)
        return self.value or other.value

    def __and__(self, other):
        if self.name != 'bool' or other.name != 'bool':
            error.exit("'and' can be used only with bools!", error.Err_codes.wrong_operand_types.value)
        return self.value and other.value

    def __not__(self):
        if self.name != 'bool':
            error.exit("'not' can be used only with bool!", error.Err_codes.wrong_operand_types.value)
        return not self.value
