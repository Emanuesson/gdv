#!/bin/bash
# Run this to generate all the initial makefiles, etc.

#TODO: write file

gtkdocize

autoreconf -fi
./configure && make
