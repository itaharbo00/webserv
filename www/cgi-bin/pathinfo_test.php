#!/usr/bin/php-cgi
<?php
header("Content-Type: text/plain");
echo "PATH_INFO test\n";
echo "==============\n";
echo "PATH_INFO: " . (isset($_SERVER['PATH_INFO']) ? $_SERVER['PATH_INFO'] : '(empty)') . "\n";
echo "SCRIPT_NAME: " . (isset($_SERVER['SCRIPT_NAME']) ? $_SERVER['SCRIPT_NAME'] : '(empty)') . "\n";
echo "QUERY_STRING: " . (isset($_SERVER['QUERY_STRING']) ? $_SERVER['QUERY_STRING'] : '(empty)') . "\n";
?>
