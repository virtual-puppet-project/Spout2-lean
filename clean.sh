#!/bin/bash

echo "Deleting dll, lib, and exe files"
find . -type f \( -iname \*.dll -o -iname \*.lib -o -iname \*.exe \) -delete

