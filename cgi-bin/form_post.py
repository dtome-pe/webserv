import os
import sys

input_string = sys.stdin.read()

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
</body>
</html>''')
