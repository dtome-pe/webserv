import os
import sys

fileContent = sys.stdin.read()
uploadDir = os.environ['UPLOAD_DIR']

fileName = os.environ['FILENAME']
filePath = uploadDir + fileName

if os.path.isfile(filePath):
    f = open(filePath, "w")
    f.write(fileContent)
    f.close()
    os.chmod(filePath, 0o664)
    print("Status: 204 No Content")

elif not os.path.isfile(filePath):
    f = open(filePath, "x")
    f.write(fileContent)
    f.close()
    os.chmod(filePath, 0o664)
    print("Status: 201 Created")
