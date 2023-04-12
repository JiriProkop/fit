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
parser.add_argument('--source', type=str,
                    help="Source code file's destination.")
parser.add_argument('--input', type=str, help="destination of the input file - for read command")
args = parser.parse_args()


def check_file_access(file):
    return os.access(file, os.R_OK)

# TODO input read
if not args.source and not args.input:
    error.exit("destination of both source and input is NOT given!",
               error.Err_codes.missing_param.value)
elif not args.source:
    if not check_file_access(args.input):
        error.exit("couldn't open the file!",
                   error.Err_codes.input_file_err.value)
    try:
        tree = ET.parse(sys.stdin)
    except:
        error.exit("could not parse the XML!",
                   error.Err_codes.xml_wrong_format.value)
elif not args.input:
    if not check_file_access(args.source):
        error.exit("couldn't open the file!", error.Err_codes.source_err.value)
    try:
        tree = ET.parse(args.source)
    except:
        error.exit("could not parse the XML!",
                   error.Err_codes.xml_wrong_format.value)
else:
    try:
        tree = ET.parse(args.source)
    except:
        error.exit("could not parse the XML!",
                   error.Err_codes.xml_wrong_format.value)

# XML is now parsed in tree, regardless if it comes from stdin or file
root = tree.getroot()

if root.attrib["language"] != "IPPcode23":
    error.exit("IPPcode23 header required!",
               error.Err_codes.xml_bad_structure.value)

# sorts the child elements by value of order
root[:] = sorted(root, key=lambda child: int(child.attrib["order"]))

# get all labels and check xml structure
line = 1
orders = []
for child in root:
    # check uniqueness of the order num
    order = int(child.attrib["order"])
    if order in orders:
        error.exit("Duplicit order values!", error.Err_codes().xml_bad_structure.value)
    else:
        orders.append(order)
    # sorts the args by it's number
    child[:] = sorted(child, key=lambda arg: int(arg.tag[3:]))
    if child.attrib["opcode"] == "LABEL":
        instr.Label().check_structure(child)
        instr.Label().check_sem(child)
        instr.Label().exec(child)
    line += 1
    if child.attrib["opcode"] == "MOVE":
        instr.Move().check_structure(child)
        instr.Move().check_sem(child)
        

print("DONE")
stringg = instr.Type("string", "řetězec\\032s\\032lomítkem\\032\\092\\032a\\010novým\\035řádkem")
instruction_switch = {
    #tady odkazy na jednotlive instrukce 
}



# TODO vytvor classu pro kazdou instrukci
# projed znovu a provadej postupne instrukce (while - kvuli skakani)
