---
title: DTOP
section: 1
header: User Manual
footer: dtop 0.1.0
date: March 12, 2021
---

# NAME

dtop - display distributed linux process

# SYNOPSIS

**dtop** [OPTION] [ARGUMENT]...

**dtop** -h

**dtop** -s [OPTION] [SERVER OPTION] [ARGUMENT]...

**dtop** -c [OPTION] [CLIENT OPTION] [ARGUMENT]...

# DESCRIPTION

**dtop** is a distributed top tool. It display linux process on all machine of client who are connected to the server.
It allows user to create server and to connect on a server.

# GENERAL OPTIONS

**-h**
: Display a friendly help message.

**-s**
: Create a server.

**-c**
: Create a client.

**-4**
: Using only ipv4.

**-6**
: Using only ipv6.

## SERVER OPTIONS

**-u**
: Select the max number of user.

**-p**
: Select the port of the server.

## CLIENT OPTIONS

**-i**
: connection to the server on the fiven ip.

**-p**
: connection to the server on the given port.

# EXAMPLES
**dtop -h**
: Displays help.

**dtop -s -4 -u 10 -p 8080**
: Create a server using only ipv4 for 10 users maximum on port 8080.

**dtop -c -4 -i 10.0.2.15 -p 8080**
: Create a client which connect to server 10.0.2.15 on port 8080 and use only ipv4.

# AUTHORS
Written by Sholde and m4ssi.

# BUGS
Submit bug reports online at: <https://github.com/aisetop/dtop/issues>

# SEE ALSO
## SERVER - CLIENT
getaddrinfo(3), socket(3), bind(3), select(3), accept(3), connect(3)

## SENSOR
"<proc/readproc.h>", readproc(3), uname(3) 

## DISPLAY
printf(3)
