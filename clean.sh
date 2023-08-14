#!/bin/bash

echo "Deleting dll and lib files"
find . -type f \( -iname \*.dll -o -iname \*.lib \) -delete

