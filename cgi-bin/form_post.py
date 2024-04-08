#!/usr/bin/python3
import os
import sys
import urllib.parse

input_string = sys.stdin.read()
print(input_string)
print ("Content-type: text/html; charset=UTF-8")
print()
print ('''<!DOCTYPE html>
<html>
<head>
<title>Received POST Data</title>
</head>
<body>
<h1>Info passed by stdin was:</h1>
<p> ''')
print (input_string)
print ('''</p>
<p>AFTER DECODING: </p>''')
print (urllib.parse.parse_qs(input_string))    
print('''
</body>
</html>''')
