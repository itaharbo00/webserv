#!/usr/bin/php-cgi
<?php
// Script CGI PHP simple - Test GET
header("Content-Type: text/html; charset=UTF-8");

echo "<!DOCTYPE html>\n";
echo "<html lang=\"en\">\n";
echo "<head>\n";
echo "    <meta charset=\"UTF-8\">\n";
echo "    <title>PHP CGI Test - GET</title>\n";
echo "    <style>\n";
echo "        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
echo "        h1 { color: #667eea; }\n";
echo "        .info { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n";
echo "        .label { font-weight: bold; color: #333; }\n";
echo "    </style>\n";
echo "</head>\n";
echo "<body>\n";
echo "    <h1>🐘 PHP CGI - GET Request Test</h1>\n";
echo "    <div class=\"info\">\n";
echo "        <p><span class=\"label\">PHP Version:</span> " . phpversion() . "</p>\n";
echo "        <p><span class=\"label\">Current Time:</span> " . date('Y-m-d H:i:s') . "</p>\n";
echo "        <p><span class=\"label\">Request Method:</span> " . $_SERVER['REQUEST_METHOD'] . "</p>\n";
echo "        <p><span class=\"label\">Query String:</span> " . ($_SERVER['QUERY_STRING'] ?: '(empty)') . "</p>\n";
echo "        <p><span class=\"label\">Script Name:</span> " . $_SERVER['SCRIPT_NAME'] . "</p>\n";
echo "        <p><span class=\"label\">Server Protocol:</span> " . $_SERVER['SERVER_PROTOCOL'] . "</p>\n";
echo "    </div>\n";
echo "</body>\n";
echo "</html>\n";
?>
