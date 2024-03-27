<?php
$queryString = $_SERVER['QUERY_STRING'];
$body = "<!DOCTYPE html>
<html>
<head>
<title>Received POST Data</title>
</head>
<body>
<h1>Query string was:</h1>
<p>$queryString</p>
</body>
</html>";

// Calculate content length
$length = strlen($body);

// Set HTTP headers
header("Content-type: text/html; charset=UTF-8");

// Output the body
echo $body;
?>
