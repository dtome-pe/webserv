#!/usr/bin/env python3
import cgi
import os

# Set the directory where uploaded files will be saved
UPLOAD_DIR = "/path/to/upload/directory"

# Create upload directory if it doesn't exist
os.makedirs(UPLOAD_DIR, exist_ok=True)

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get filename
file_item = form["file"]
if file_item.filename:
    # Construct the filename to save on the server
    filename = os.path.join(UPLOAD_DIR, os.path.basename(file_item.filename))
    # Write the file to the server
    with open(filename, 'wb') as f:
        f.write(file_item.file.read())

# Print HTML response
print("Content-type: text/html\n")
print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head>")
print("<meta charset='UTF-8'>")
print("<meta name='viewport' content='width=device-width, initial-scale=1.0'>")
print("<title>File Upload</title>")
print("</head>")
print("<body>")
print("<h2>File Uploaded Successfully</h2>")
print("<p>The file has been saved on the server.</p>")
print("</body>")
print("</html>")