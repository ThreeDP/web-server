#!/usr/bin/php-cgi
<?php
// CGI scripts typically need to set the content-type header
header("Content-Type: text/html");

// You can access CGI environment variables with $_SERVER in PHP
$method = $_SERVER['REQUEST_METHOD'];
$query_string = $_SERVER['QUERY_STRING'];

// Simple HTML response
echo "<html>";
echo "<head><title>PHP CGI Script</title></head>";
echo "<body>";
echo "<h1>Hello from PHP CGI!</h1>";
echo "<p>Request Method: " . htmlspecialchars($method) . "</p>";
echo "<p>Query String: " . htmlspecialchars($query_string) . "</p>";
echo "</body>";
echo "</html>";
?>
