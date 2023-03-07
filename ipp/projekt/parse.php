<?php
ini_set('display_errors', 'stderr');

/*
    @enum Return codes for errors.
*/
enum err_codes: int {
    case err_ok = 0;
    case err_parameter = 10;
    case err_in = 11;           // Couldn't access input file.
    case err_out = 12;          // Output file error.
    case err_header = 21;       // Wrong or missing IPPcode header.
    case err_wrong_code = 22;   // Unknown or wrong given code.
    case err_syn_or_lex = 23;   // Other lexem or syntax error.
    case err_internal = 99;
}

/*
    Function for printing help.
*/
function help(){
    echo "parse.php [--help]\n";
    echo "      Takes IPPcode23 code from STDIN and performes static analysis.\n";
    echo "      Then prints XML description of it to STDOUT.\n";
    exit(err_codes::err_ok->value);
}

/*
    @param <varieble> to be checked
    @return true if given var is wrong
*/
function check_var($var) {
    return (preg_match("/^(((TF)|(GF)|(LF))(@[a-zA-Z_\-\$&\%\*!\?][a-zA-Z_\-\$&\%\*!\?\d]*))$/", $var) !== 1) ? true : false;
}

/*
    @param <constant> to be checked
    @return true if given var is wrong
*/
function check_constant($const) {
    return (preg_match("/^((string@([^\\\\\x01-\x20]|(\\\\\d\d\d))*)|(int@[^\s][+-]?((0x[a-f\d]*)|(0o?\d*)|\d*))|(nil@nil)|(bool@((true)|(false))))$/",
    $const) !== 1) ? true : false;
}

/*
    @param <symbol> to be checked
    @return true if given var is wrong
*/
function check_symbol($symb) {
    return check_var($symb) && check_constant($symb);
}

/*
    @param <label> to be checked
    @return true if given var is wrong
*/
function check_label($label) {
    return (preg_match("/^([a-zA-Z_\-\$&\%\*!\?][a-zA-Z_\-\$&\%\*!\?\d]*)$/", $label) !== 1) ? true : false;
}

/*
    @param <symbol>
    @return IPPcode23 type of given symbol
*/
function get_type($symb) {
    if(preg_match("/int@/", $symb) === 1) {
        return "int";
    } else if (preg_match("/bool@/", $symb) === 1) {
        return "bool";
    } else if (preg_match("/string@/", $symb) === 1) {
        return "string";
    } else if (preg_match("/nil@/", $symb) === 1) {
        return "nil";
    } else if (preg_match("/(int)|(string)|(bool)/", $symb) === 1) {
        return "type";
    } else {
        return "var";
    }
}

/*
    This function will strip the type('@' too) of the given symbol if
    it's 'string', 'int', 'nil' or 'bool'. Does nothing otherwise.

    @param <symbol>
    @return value of given parameter
*/
function get_value($symb) {
    switch (get_type($symb)) {
        case "string":
        case "int":
        case "nil":
        case "bool":
            return substr($symb, strpos($symb, "@") + 1, strlen($symb) - 1);
        default:
            return  $symb;
    }
}

if($argc == 2 && strcmp("--help", $argv[1]) === 0) {
    help();
} else if($argc !== 1) {
    exit(err_codes::err_in->value);
}

$check_header = true;
$order = 1;
$xmlstr = new SimpleXMLElement('<?xml version="1.0" encoding="UTF-8"?><program></program>');
$xmlstr->addAttribute('language', 'IPPcode23');

while($line = fgets(STDIN)) {
    if(trim($line) === "") {
        continue;
    }
    $pos = strpos($line, "#");
    if($pos !== false) {
        $line = substr($line, 0, $pos);
        if($pos === 0) {
            continue;
        }
    }
    if($check_header) {
        if(strcmp(".ippcode23", strtolower(trim($line))) != 0) {
            exit(err_codes::err_header->value);
        } else {
            $check_header = false;
            continue;
        }
    }
    $line = trim($line);
    $line = explode(" ", $line);
    $line = array_combine(array_keys($line), array_pad(array_filter($line), count($line), ''));
    $line = array_filter($line);
    $CMD = trim(strtoupper($line[0]));
    if($line === "") {
        continue;
    }

    $instruction = $xmlstr->addChild('instruction');
    $instruction->addAttribute('order', $order++);
    $instruction->addAttribute('opcode', $CMD);
    switch ($CMD) {
        // CMD
        case "CREATEFRAME":
        case "POPFRAME":
        case "PUSHFRAME":
        case "RETURN":
        case "BREAK":
            if(count($line) !== 1) {
                exit(err_codes::err_syn_or_lex->value);
            }
            break;
            // CMD <label>
        case "CALL":
        case "JUMP":
        case "LABEL":
            if(check_label($line[1])|| count($line) !== 2) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = $line[1];
            $arg1->addAttribute('type', 'label');
            break;
        // CMD <var>
        case "DEFVAR":
        case "POPS":
            if(check_var($line[1]) || count($line) !== 2) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = $line[1];
            $arg1->addAttribute('type', 'var');
            break;
        // CMD <symb>
        case "WRITE":
        case "EXIT":
        case "PUSHS":
        case "DPRINT":
            if(check_symbol($line[1]) || count($line) !== 2) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = get_value($line[1]);
            $arg1->addAttribute('type', get_type($line[1]));
            break;
        // CMD <var> <type>
        case "READ":
            if(check_var($line[1]) || preg_match("/^((int)|(string)|(bool))$/", trim($line[2])) !== 1 || count($line) !== 3) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = $line[1];
            $arg1->addAttribute('type', 'var');

            $arg2 = $instruction->addChild('arg2');
            $arg2[0] = $line[2];
            $arg2->addAttribute('type', 'type');
            break;
        // CMD <var> <symb>
        case "MOVE":
        case "INT2CHAR":
        case "STRLEN":
        case "TYPE":
        case "NOT":
            if(check_var($line[1]) || check_symbol($line[2]) || count($line) !== 3) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = $line[1];
            $arg1->addAttribute('type', 'var');

            $arg2 = $instruction->addChild('arg2');
            $arg2[0] = get_value($line[2]);
            $arg2->addAttribute('type', get_type($line[2]));
            break;
        // CMD <var> <symb1> <symb2>
        case "ADD":
        case "SUB":
        case "MUL":
        case "IDIV":
        case "LT":
        case "GT":
        case "EQ":
        case "AND":
        case "OR":
        case "STRI2INT":
        case "CONCAT":
        case "GETCHAR":
        case "SETCHAR":
            if(check_var($line[1]) || check_symbol($line[2]) || check_symbol($line[3]) || count($line) !== 4) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = $line[1];
            $arg1->addAttribute('type', 'var');

            $arg2 = $instruction->addChild('arg2');
            $arg2[0] = get_value($line[2]);
            $arg2->addAttribute('type', get_type($line[2]));

            $arg3 = $instruction->addChild('arg3');
            $arg3[0] = get_value($line[3]);
            $arg3->addAttribute('type', get_type($line[3]));
            break;
        // CMD <label> <symb1> <symb2>
        case "JUMPIFEQ":
        case "JUMPIFNEQ":
            if(check_label($line[1]) || check_symbol($line[2]) || check_symbol($line[3]) || count($line) !== 4) {
                exit(err_codes::err_syn_or_lex->value);
            }
            $arg1 = $instruction->addChild('arg1');
            $arg1[0] = $line[1];
            $arg1->addAttribute('type', 'label');

            $arg2 = $instruction->addChild('arg2');
            $arg2[0] = get_value($line[2]);
            $arg2->addAttribute('type', get_type($line[2]));

            $arg3 = $instruction->addChild('arg3');
            $arg3[0] = get_value($line[3]);
            $arg3->addAttribute('type', get_type($line[3]));
            break;
        default:
            exit(err_codes::err_wrong_code->value);
    }
} // end of while
$dom = new DOMDocument();
$dom->formatOutput = true;
$dom->preserveWhiteSpace = false;
$dom->loadXML($xmlstr->asXML());
echo $dom->saveXML();

exit(err_codes::err_ok->value);
?>