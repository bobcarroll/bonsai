#!/bin/sh

if [[ $# != 2 ]]; then
    echo "USAGE: runtest.sh <command> <output file>"
    exit 1;
fi

$1 &> $2
