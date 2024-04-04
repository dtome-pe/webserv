import os
import sys

input_string = os.environ['QUERY_STRING']

print ("Content-type: text/html; charset=UTF-8")
print()
print ('''<!DOCTYPE html>
<html>
<head>
<title>Received POST Data</title>
</head>
<body>
<h1>Info passed by query string was:</h1>
<p> ''')
print (input_string)
print ('''</p>
</body>
</html>''')
