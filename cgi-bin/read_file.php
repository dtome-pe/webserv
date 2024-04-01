<?php
$fileContents = file_get_contents($_FILES['file']['tmp_name']);

$body = "<!DOCTYPE html>
<html>
<head>
<title>Received POST Data</title>
</head>
<body>
<h1>Info passed by stdin was:</h1>
<p>$fileContents</p>
</body>
</html>";

header("Content-type: text/html; charset=UTF-8");

echo $body;
?>
