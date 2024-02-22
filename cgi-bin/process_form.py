#!/usr/bin/env python3
import cgi

# Set the content type to HTML
print("Content-type: text/html\n")

# Get form data
form = cgi.FieldStorage()

# Retrieve values from the form
name = form.getvalue('name')
email = form.getvalue('email')
message = form.getvalue('message')

print("Content-Type: text/html") 
# HTML response
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