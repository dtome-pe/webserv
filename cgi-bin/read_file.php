<?php
echo("Content-type: text/html\n");

echo "<!DOCTYPE html>";
echo "<html>";
echo "<head>";
echo "<title>Received POST Data</title>";
echo "</head>";
echo "<body>";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Check if CONTENT_LENGTH is set
    if (isset($_SERVER['CONTENT_LENGTH'])) {
        // Read the specified number of bytes from stdin
        $contentLength = (int)$_SERVER['CONTENT_LENGTH'];
        $postData = fread(STDIN, $contentLength);

        // Print out the POST data
        echo "<h1>Received POST data:</h1>";
        echo "<pre>";
        echo htmlspecialchars($postData); // Convert special characters to HTML entities to prevent XSS attacks
        echo "</pre>";
    } else {
        // If CONTENT_LENGTH is not set, return an error
        echo "<h1>ERROR:</h1>";
        echo "<p>CONTENT_LENGTH not set.</p>";
    }
} else {
    // If not a POST request, return an error
    echo "<h1>ERROR:</h1>";
    echo "<p>Only POST requests are supported.</p>";
}

echo "</body>";
echo "</html>";
?>
