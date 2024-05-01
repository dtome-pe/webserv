#!/usr/bin/python3
import os
import sys
import cgi
import cgitb

cgitb.enable()

try:
    form = cgi.FieldStorage(keep_blank_values=True)
    fileContents = form['file'].file.read().decode('utf-8')

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
    print (fileContents)
    print ('''<pre>
    </body>
    </html>''')

    sys.exit()

except Exception as e:
    print("Status: 500 Internal Server Error")
