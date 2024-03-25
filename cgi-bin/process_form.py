#!/usr/bin/env python3
import cgi
import os

# Set the content type to HTML
print("Content-type: text/html\n")

# Get form data
form = cgi.FieldStorage()

# Retrieve values from the form
name = form.getvalue('name')
email = form.getvalue('email')
message = form.getvalue('message')

# HTML response

print(os.environ)

print("<html>")
print("<head>")
print("<title>Form Submission Result</title>")
print("</head>")
print("<body>")

# Display submitted data
print("<h2>Form Submission Result:</h2>")
print("<p><strong>Name:</strong> {}</p>".format(name))
print("<p><strong>Email:</strong> {}</p>".format(email))
print("<p><strong>Message:</strong> {}</p>".format(message))

print("</body>")
print("</html>")