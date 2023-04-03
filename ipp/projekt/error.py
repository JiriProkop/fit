from enum import Enum
import sys

class Err_codes(Enum):
    missing_param = 10
    source_err = 11
    input_file_err = 12
    xml_wrong_format = 31
    xml_bad_structure = 32
    semantic_check_err = 52
    wrong_operand_types = 53
    nonexistant_var_used = 54
    frame_doesnt_exists = 55
    missing_value = 56
    bad_operand_value = 57 # such as 0 division
    invalid_string_operation = 58
    internal_err = 99

def exit(msg, err_code):
    print("Error: " + msg, file=sys.stderr)
    sys.exit(err_code)