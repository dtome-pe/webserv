import os
import sys

print("Content-type: text/plain")
print()
for name, value in os.environ.items():
    print("{0}: {1}".format(name, value))