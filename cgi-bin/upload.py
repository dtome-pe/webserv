#!/usr/bin/python3
import os
import sys
import cgi
import cgitb

cgitb.enable()


try:
    form = cgi.FieldStorage(keep_blank_values=True)
    if os.environ['UPLOAD_DIR'] == "none":
        print("Status: 403 Forbidden")
        sys.exit()
    if form['file'].filename:
        uploadDir = os.environ['UPLOAD_DIR'] + "/" + form['file'].filename
        f = open(uploadDir, "wb")
        fileContents = form['file'].file.read()
        f.write(fileContents)
        f.close()
        print("Content-Type: text/html")
        print()
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

except Exception as e:
    print("Status: 500 Internal Server Error")
