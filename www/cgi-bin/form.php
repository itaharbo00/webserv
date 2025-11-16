#!/usr/bin/php-cgi
<?php
// Script CGI PHP - Test POST avec formulaire
header("Content-Type: text/html; charset=UTF-8");

// Lire le corps de la requête POST
$postData = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = file_get_contents('php://input');
    parse_str($input, $postData);
}

echo "<!DOCTYPE html>\n";
echo "<html lang=\"en\">\n";
echo "<head>\n";
echo "    <meta charset=\"UTF-8\">\n";
echo "    <title>PHP CGI Test - POST Form</title>\n";
echo "    <style>\n";
echo "        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
echo "        h1 { color: #667eea; }\n";
echo "        .container { max-width: 600px; margin: 0 auto; }\n";
echo "        .form-box, .result-box { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 20px; }\n";
echo "        label { display: block; margin-bottom: 5px; font-weight: bold; color: #333; }\n";
echo "        input { width: 100%; padding: 8px; margin-bottom: 15px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }\n";
echo "        button { background: #667eea; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }\n";
echo "        button:hover { background: #5568d3; }\n";
echo "        .success { background: #d4edda; color: #155724; padding: 15px; border-radius: 4px; border: 1px solid #c3e6cb; }\n";
echo "    </style>\n";
echo "</head>\n";
echo "<body>\n";
echo "    <div class=\"container\">\n";
echo "        <h1>📝 PHP CGI - POST Form Test</h1>\n";

if ($_SERVER['REQUEST_METHOD'] === 'POST' && !empty($postData)) {
    echo "        <div class=\"result-box\">\n";
    echo "            <div class=\"success\">\n";
    echo "                <h3>✅ Form Submitted Successfully!</h3>\n";
    echo "                <p><strong>Name:</strong> " . htmlspecialchars($postData['name'] ?? 'N/A') . "</p>\n";
    echo "                <p><strong>Email:</strong> " . htmlspecialchars($postData['email'] ?? 'N/A') . "</p>\n";
    echo "                <p><strong>Content-Length:</strong> " . $_SERVER['CONTENT_LENGTH'] . " bytes</p>\n";
    echo "                <p><strong>Content-Type:</strong> " . $_SERVER['CONTENT_TYPE'] . "</p>\n";
    echo "            </div>\n";
    echo "        </div>\n";
}

echo "        <div class=\"form-box\">\n";
echo "            <form method=\"POST\" action=\"" . $_SERVER['SCRIPT_NAME'] . "\">\n";
echo "                <label for=\"name\">Name:</label>\n";
echo "                <input type=\"text\" id=\"name\" name=\"name\" required>\n";
echo "                <label for=\"email\">Email:</label>\n";
echo "                <input type=\"email\" id=\"email\" name=\"email\" required>\n";
echo "                <button type=\"submit\">Submit</button>\n";
echo "            </form>\n";
echo "        </div>\n";
echo "    </div>\n";
echo "</body>\n";
echo "</html>\n";
?>
