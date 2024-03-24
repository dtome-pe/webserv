<?php

// Check if POST data is received
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Check if CONTENT_LENGTH is set
        echo "test\n";
    if (isset($_SERVER['CONTENT_LENGTH'])) {
        // Read the specified number of bytes from stdin
        $contentLength = (int)$_SERVER['CONTENT_LENGTH'];
        $postData = fread(STDIN, $contentLength);

        // Print out the POST data
        echo "Received POST data:\n";
        echo $postData;
    } else {
        // If CONTENT_LENGTH is not set, return an error
        echo "CONTENT_LENGTH not set.";
    }
} else {
    // If not a POST request, return an error
    echo "Only POST requests are supported.";
}
?>
