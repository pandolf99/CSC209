#!/usr/bin/env bash
./echo_arg > echo_out.txt
./echo_stdin < echo_stdin.c
./count 210 | wc -m
ls -1S | ./echo_stdin
