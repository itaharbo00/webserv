#!/bin/bash
# Script CGI Shell simple - Test GET

echo "Content-Type: text/html; charset=UTF-8"
echo ""

cat << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Shell CGI Test - GET</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }
        h1 { color: #4eaa25; }
        .info { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .label { font-weight: bold; color: #333; }
    </style>
</head>
<body>
    <h1>🐚 Shell CGI - GET Request Test</h1>
    <div class="info">
        <p><span class="label">Shell:</span> $SHELL</p>
        <p><span class="label">Current Time:</span> $(date '+%Y-%m-%d %H:%M:%S')</p>
        <p><span class="label">Request Method:</span> $REQUEST_METHOD</p>
        <p><span class="label">Query String:</span> ${QUERY_STRING:-(empty)}</p>
        <p><span class="label">Script Name:</span> $SCRIPT_NAME</p>
        <p><span class="label">Server Protocol:</span> $SERVER_PROTOCOL</p>
    </div>
</body>
</html>
EOF
