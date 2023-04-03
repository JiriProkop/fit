import argparse
import sys
import os
import instruction as instr
import xml.etree.ElementTree as ET
import error


class CustomArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        print(message, file=sys.stderr)
        self.exit(error.Err_codes.missing_param.value)


# arg parsing
parser = CustomArgumentParser(description='IPP23 interpret.')
parser.add_argument('--source', type=str,
                    help="Source code file's destination.")
parser.add_argument('--input', type=str,
                    help="destination of the input file - for read command")
args = parser.parse_args()


def check_file_access(file):
    return os.access(file, os.R_OK)


if not args.source and not args.input:
    error.exit("destination of both source and input is NOT given!",
               error.Err_codes.missing_param.value)
elif not args.source:
    if not check_file_access(args.input):
        error.exit("couldn't open the file!",
                   error.Err_codes.input_file_err.value)
    print("source ze stdin")
    try:
        tree = ET.parse(sys.stdin)
    except:
        error.exit("could not parse the XML!",
                   error.Err_codes.xml_wrong_format.value)
elif not args.input:
    if not check_file_access(args.source):
        error.exit("couldn't open the file!", error.Err_codes.source_err.value)
    print("inputze STDIN.")
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

global_frame = {}


root = tree.getroot()

if root.attrib["language"] != "IPPcode23":
    error.exit("IPPcode23 header required!",
               error.Err_codes.xml_bad_structure.value)

root[:] = sorted(root, key=lambda child: int((child.attrib["order"])))

# get all labels and check xml structure
iter = 0
for child in root:
    if child.attrib["opcode"] == "LABEL":
        instr.Label().check_structure(child)
        instr.Label.check_sem(child)
        instr.Label.exec(child, iter)

    iter += 1
    # print(child.attrib)


# projed jednou - kontrola formatu a ulozeni labelu
# TODO vytvor classu pro instrukce
# projed znovu a provadej postupne instrukce (while - kvuli skakani)
