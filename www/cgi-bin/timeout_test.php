#!/usr/bin/php-cgi
<?php
// Script CGI qui prend trop de temps
header("Content-Type: text/plain");
echo "Starting long process...\n";
flush();

sleep(35); // Dépasse le timeout de 30s

echo "This should not be reached\n";
?>
