#!/usr/bin/python3
import os
import sys
import urllib.parse

try:
    input_string = os.environ['QUERY_STRING']

    print ("Content-Type: text/html; charset=UTF-8")
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
    <p>AFTER DECODING: </p>''')
    print (urllib.parse.parse_qs(input_string))    
    print('''
    </body>
    </html>''')

except Exception as e:
    print("Status: 500 Internal Server Error")
