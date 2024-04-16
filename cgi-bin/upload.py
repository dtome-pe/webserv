#!/usr/bin/python3
import os
import sys
import cgi
import cgitb

cgitb.enable()



form = cgi.FieldStorage(keep_blank_values=True)

print("Content-type: text/html")
print()

if form['file'].filename:
    uploadDir = os.environ['UPLOAD_DIR'] + "/" + form['file'].filename
    f = open(uploadDir, "wb")
    f.write(form['file'].file.read())
    f.close()
    print (f'''<!DOCTYPE html>
    <html>
    <head>
    <title>UPLOAD FILE</title>
    </head>
    <body>
    <h1>FILE UPLOADED CORRECTLY AT {uploadDir} </h1>
    </body>
    </html>''')

else:
    print ('''<!DOCTYPE html>
    <html>
    <head>
    <title>UPLOAD FILE</title>
    </head>
    <body>
    <h1>PLEASE, GO BACK AND SELECT A FILE</h1>
    </body>
    </html>''')
