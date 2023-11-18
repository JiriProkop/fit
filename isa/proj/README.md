# TFTP server and client

This project encompasses the development of a TFTP (Trivial File Transfer Protocol) suite consisting of a TFTP server (tftp-server) and a TFTP client (tftp-client). The TFTP server is designed to receive and store files from remote clients, while the TFTP client facilitates the downloading and uploading of files to a remote server.

## Project information

- Author name: Jiří Prokop
- Author login: xproko47
- Date: 17. 11. 2023

### List of all the files

- tftp-client.c
- tftp-client.h
- tftp-server.c
- tftp-server.h
- tftp.c
- tftp.h
- Makefile
- README.md
- manual.pdf

## Limitations

Port number 0 is the default and cannot be entered as an argument due to error handling in `strtoul`.

## How to run

### tftp-server

```
tftp-server [-p port] root_dirpath

-h, --help:     Print this help message.

-p:             Specify the local port on which the server to run on.
                The default is 69.

root_dirpath:   Path to the directory where incoming files will be saved.
```

Example: `./tftp-server -p 7999 .`

If you choose to use the default port, it is required to run it with sudo permissions.
That's because the port 69 is within
[well-known ports](https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers).

### tftp-client

```
tftp-client -h hostname [-p port] [-f filepath] -t dest_filepath

--help:     Print this help message.

-h:             IP address/domain name of the remote server.

-p:             Port of the remote server. Default is 69.

-f:             Path to the file to be downloaded from the server (download).
                If not specified, content from stdin will be used (upload).

-t:             Path under which the file will be stored on the remote server or locally.

```

Example(upload): `./tftp-client -h localhost -p 7999 -t some_name.txt <myfile.txt`
