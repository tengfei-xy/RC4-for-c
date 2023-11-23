#!/bin/bash
gcc -g -liconv -lstdc++   -o "test" ./*.c || exit "$?"
./test
