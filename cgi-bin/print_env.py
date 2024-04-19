#!/usr/bin/python3
import os
import sys

print("Content-Type: text/plain")
print()
for name, value in os.environ.items():
    print("{0}: {1}".format(name, value))