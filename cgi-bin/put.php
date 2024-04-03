<?php

// Get the file content from the request body
$fileContent = file_get_contents("php://stdin");

// Specify the directory where the file will be saved
$uploadDir = $_ENV['UPLOAD_DIR'];

// Specify the file name (you may want to use a different naming convention)
$fileName = $_ENV['FILENAME'];

// Specify the full path of the file
$filePath = $uploadDir . $fileName;

// Check if the file already exists
if (file_exists($filePath)) {
    // File already exists, so we are updating it
    // Overwrite the existing file with the new content
    if (file_put_contents($filePath, $fileContent) !== false) {
        // Set permissions for the uploaded file (optional)
        chmod($filePath, 0644); // Adjust permissions as needed
        exit(204); // Set HTTP response code to 204 (No Content)
    } else {
       // echo "Error: Failed to update file.\n";
        exit(0); // Internal Server Error
        
    }
} else {
    // File does not exist, so we are creating a new file
    // Save the file to the specified directory
    if (file_put_contents($filePath, $fileContent) !== false) {
        // Set permissions for the uploaded file (optional)
        chmod($filePath, 0644); // Adjust permissions as needed
        exit(201); // Set HTTP response code to 201 (Created)
        
    } else {
        exit(0); // Internal Server Error
    }
}
?>
