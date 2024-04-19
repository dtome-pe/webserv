#!/usr/bin/python3
import os
import sys
import cgi

form = cgi.FieldStorage(keep_blank_values=True)

print ("Content-Type: text/html; charset=UTF-8")

print()

print ('''<!DOCTYPE html>
<html>
<head>
<title>Read FILE</title>
</head>
<body>
<h1>File content is:</h1>
<pre> ''')
print (form['file'].file.read().decode('utf-8'))
print ('''<pre>
</body>
</html>''')

sys.exit()
