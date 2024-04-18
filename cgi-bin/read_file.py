#!/usr/bin/python3
import os
import sys

fileContent = sys.stdin.read()

print ("Content-type: text/html; charset=UTF-8")

print()

print ('''<!DOCTYPE html>
<html>
<head>
<title>Read FILE</title>
</head>
<body>
<h1>File content is:</h1>
<p> ''')
print (fileContent)
print ('''</p>
</body>
</html>''')

sys.exit()
