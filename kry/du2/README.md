# KRY Project 2

## Description
This project implements a basic MAC (Message Authentication Code) generator using a custom implementation of the SHA-256 hash algorithm and a secret key. It also includes functionality for verifying MACs and performing a length extension attack on MAC-protected messages.


## Usage
After compilation, you can run the program with following parameters:
```
./kry [OPTION]...
Options:
  -c              Compute hash and print it
  -s              Compute MAC and print it
  -v              Verify MAC
  -e              Do a length extension attack
  -k              Key for MAC computation
  -m              Given MAC for verification
  -n              Length of the key
  -a              String to append for length extension attack
```

Text for which you want to perform hash is given via STDIN.
You can run the program with any of these combinations:
```
./kry -c                                # Compute hash and print it
./kry -s -k KEY                         # Compute MAC with the specified key and print it
./kry -v -k KEY -m MAC                  # Verify the MAC using the specified key and MAC
./kry -e -n LENGTH -a STRING -m MAC     # Perform a length extension attack with the given parameters
```

Examples:
```bash
$ echo -ne "zprava" | ./kry -c
$ echo -ne "zprava" | ./kry -s -k heslo
$ echo -ne "zprava" | ./kry -v -k heslo -m 23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18e
$ echo -ne "message" | ./kry -v -k password -m 23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18e
$ echo -ne "zprava" | ./kry -e -n 5 -a ==message -m 23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18e
```

## Disclaimer
Works only on machines where BYTE = 8 bits and where little-endian architecture is used.
