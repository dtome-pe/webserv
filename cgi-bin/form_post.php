<?php
if ($stdin = fopen('php://input', 'r')) {
    // Read all lines from stdin
    $queryString = '';
    while ($line = fgets($stdin)) {
        $queryString .= $line;
    }
    fclose($stdin);
} else {
    // If stdin is not available, set an error message
    $queryString = "No input received from stdin";
}

$body = "<!DOCTYPE html>
<html>
<head>
<title>Received POST Data</title>
</head>
<body>
<h1>Info passed by stdin was:</h1>
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