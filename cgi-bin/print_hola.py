import sys

try:
    print("Content-Type: text/plain")
    print()
    print("hola")
    sys.exit()

except Exception as e:
    print("Status: 500 Internal Server Error")