#!/bin/bash

gcc -g sgc.c -o sgc && valgrind --leak-check=full ./sgc
