import argparse
import sys
import os
import instructions as instr
import xml.etree.ElementTree as ET
import error


class CustomArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        print(message, file=sys.stderr)
        self.exit(error.Err_codes.missing_param)


# arg parsing
parser = CustomArgumentParser(description='IPP23 interpret.')
parser.add_argument('--source', type=str, help="Source code file's destination.")
parser.add_argument('--input', type=str, help="destination of the input file - for read command")
args = parser.parse_args()


def check_file_access(file):
    return os.access(file, os.R_OK)


if not args.source and not args.input:
    error.exit("destination of both source and input is NOT given!",
               error.Err_codes.missing_param.value)
elif not args.source:
    # source from stdin
    try:
        tree = ET.parse(sys.stdin)
    except:
        error.exit("could not parse the XML!", error.Err_codes.xml_wrong_format.value)
    # input from file
    try:
        input_text = open(args.input, 'r')
        input_text = input_text.readline
    except:
        error.exit("couldn't open the input file!", error.Err_codes.input_file_err.value)
elif not args.input:
    # source from file
    if not check_file_access(args.source):
        error.exit("couldn't open the file!", error.Err_codes.source_err.value)
    try:
        tree = ET.parse(args.source)
    except:
        error.exit("could not parse the XML!", error.Err_codes.xml_wrong_format.value)
    # input from stdin
    input_text = input
else:
    # source from file
    if not check_file_access(args.source):
        error.exit("couldn't open the file!", error.Err_codes.source_err.value)
    try:
        tree = ET.parse(args.source)
    except:
        error.exit("could not parse the XML!", error.Err_codes.xml_wrong_format.value)
    # input from file
    try:
        input_text = open(args.input, 'r')
        input_text = input_text.readline
    except:
        error.exit("couldn't open the input file!", error.Err_codes.input_file_err.value)


# XML is now parsed in tree, regardless if it comes from stdin or file
root = tree.getroot()

if root.attrib["language"] != "IPPcode23":
    error.exit("IPPcode23 header required!", error.Err_codes.xml_bad_structure.value)

# sorts the child elements by value of order
try:
    root[:] = sorted(root, key=lambda child: int(child.attrib["order"]))
except:
    error.exit("nonvalid order!", error.Err_codes.xml_bad_structure.value)
# give necessery data to instructions
instr.Instruction.root = root
instr.Instruction.input = input_text

instruction_switch = {
    'MOVE': instr.Move,
    'CREATEFRAME': instr.Createframe,
    'PUSHFRAME': instr.Pushframe,
    'POPFRAME': instr.Popframe,
    'DEFVAR': instr.Defvar,
    'CALL': instr.Call,
    'RETURN': instr.Return,
    'PUSHS': instr.Pushs,
    'POPS': instr.Pops,
    'ADD': instr.Add,
    'SUB': instr.Sub,
    'MUL': instr.Mul,
    'IDIV': instr.Idiv,
    'LT': instr.Lt,
    'GT': instr.Gt,
    'EQ': instr.Eq,
    'AND': instr.And,
    'OR': instr.Or,
    'NOT': instr.Not,
    'INT2CHAR': instr.Int2char,
    'STRI2INT': instr.Stri2int,
    'READ': instr.Read,
    'WRITE': instr.Write,
    'CONCAT': instr.Concat,
    'STRLEN': instr.Strlen,
    'GETCHAR': instr.Getchar,
    'SETCHAR': instr.Setchar,
    'TYPE': instr.Type,
    'JUMP': instr.Jump,
    'JUMPIFEQ': instr.Jumpifeq,
    'JUMPIFNEQ': instr.Jumpifneq,
    'EXIT': instr.Exit,
    'DPRINT': instr.Dprint,
    'BREAK': instr.Break,
    'LABEL': ''
}

# get all labels and check xml structure
orders = []
for child in root:
    # check uniqueness of the order num
    order = int(child.attrib["order"])
    if order in orders:
        error.exit("Duplicit order values!", error.Err_codes.xml_bad_structure.value)
    elif order < 1:
        error.exit("nonvalid order num!", error.Err_codes.xml_bad_structure.value)
    elif child.tag != 'instruction':
        error.exit("invalid instruction tag!", error.Err_codes.xml_bad_structure.value)
    else:
        orders.append(order)
    # sorts the args by it's number

    try:
        child[:] = sorted(child, key=lambda arg: int(arg.tag[3:]))
        # check arg tag numbers
        if instr.Instruction.arg_count(child) == 1:
            if int(child[0].tag[3:]) != 1:
                raise SyntaxError()
        elif instr.Instruction.arg_count(child) == 2:
            if int(child[0].tag[3:]) != 1 or int(child[1].tag[3:]) != 2:
                raise SyntaxError()
        elif instr.Instruction.arg_count(child) == 3:
            if int(child[0].tag[3:]) != 1 or int(child[1].tag[3:]) != 2 or int(child[2].tag[3:]) != 3:
                raise SyntaxError()
    except:
        error.exit("invalid argument tag!", error.Err_codes.xml_bad_structure.value)
    # strip arg.text of white characters
    for arg in child:
        arg.text = arg.text.strip()
    try:
        instruction_switch[child.attrib["opcode"].upper()]
    except:
        error.exit("nonexistant opcode!", error.Err_codes.xml_bad_structure.value)
    if child.attrib["opcode"].upper() == "LABEL":
        instr.Label().check_structure(child)
        instr.Label().check_sem(child)
        instr.Label().exec(child)

try:
    max_index = list(root).index(list(root)[-1])
except:
    # means there're no instructions
    exit()
i = 0
while i <= max_index:
    current_inst = root[i].attrib['opcode'].upper()
    if current_inst == 'LABEL':
        i += 1
        continue
    instruction_switch[current_inst].check_structure(root[i])
    instruction_switch[current_inst].check_sem(root[i])
    jump = instruction_switch[current_inst].exec(root[i])
    if jump is None:
        i += 1
    else:
        i = jump
