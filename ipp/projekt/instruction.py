import error

class Instruction:
    labels = {}

    @classmethod
    def exec(cls):
        raise NotImplementedError()

    @classmethod
    def check_structure(cls):
        raise NotImplementedError()

    @classmethod
    def check_sem(cls):
        raise NotImplementedError()


class Label(Instruction):
    @classmethod
    def exec(cls, child, line):
        cls.labels[child[0].text] = line
        print(cls.labels)

    @classmethod
    def check_structure(cls, child):
        if len(child[0].attrib) != 1 or child[0].attrib["type"] != 'label':
            error.exit("invalid label structure!", error.Err_codes.xml_bad_structure.value)

    @classmethod
    def check_sem(cls, child):
        if child[0].text in cls.labels:
            error.exit("redefinition of label!", error.Err_codes.semantic_check_err.value)

