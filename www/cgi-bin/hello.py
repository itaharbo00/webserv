#!/usr/bin/python3
import os
import sys
from datetime import datetime

# Script CGI Python simple - Test GET
print("Content-Type: text/html; charset=UTF-8\n")

print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>")
print("    <meta charset=\"UTF-8\">")
print("    <title>Python CGI Test - GET</title>")
print("    <style>")
print("        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }")
print("        h1 { color: #3776ab; }")
print("        .info { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }")
print("        .label { font-weight: bold; color: #333; }")
print("    </style>")
print("</head>")
print("<body>")
print("    <h1>🐍 Python CGI - GET Request Test</h1>")
print("    <div class=\"info\">")
print("        <p><span class=\"label\">Python Version:</span> " + sys.version + "</p>")
print("        <p><span class=\"label\">Current Time:</span> " + datetime.now().strftime('%Y-%m-%d %H:%M:%S') + "</p>")
print("        <p><span class=\"label\">Request Method:</span> " + os.environ.get('REQUEST_METHOD', 'N/A') + "</p>")
print("        <p><span class=\"label\">Query String:</span> " + (os.environ.get('QUERY_STRING') or '(empty)') + "</p>")
print("        <p><span class=\"label\">Script Name:</span> " + os.environ.get('SCRIPT_NAME', 'N/A') + "</p>")
print("        <p><span class=\"label\">Server Protocol:</span> " + os.environ.get('SERVER_PROTOCOL', 'N/A') + "</p>")
print("    </div>")
print("</body>")
print("</html>")
