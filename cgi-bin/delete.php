#!/usr/bin/php-cgi
<?php
// Check if the correct number of arguments is provided
if ($argc != 2) {
    echo "Usage: ".$_SERVER['PHP_SELF']." <file_or_directory_path>\n";
    exit(1);
}

// Get the path from command-line arguments
$path = $argv[1];

// Check if the specified path exists
if (!file_exists($path)) {
    echo "Error: The specified file or directory does not exist.\n";
    exit(1);
}

// Determine whether the path is a file or directory
if (is_file($path)) {
    // Delete the file
    if (unlink($path)) {
        echo "File '$path' has been deleted successfully.\n";
    } else {
        echo "Error: Failed to delete the file '$path'.\n";
        exit(1);
    }
} elseif (is_dir($path)) {
    // Delete the directory and its contents recursively
    if (rmdir($path)) {
        echo "Directory '$path' has been deleted successfully.\n";
    } else {
        echo "Error: Failed to delete the directory '$path'.\n";
        exit(1);
    }
} else {
    echo "Error: The specified path is neither a file nor a directory.\n";
    exit(1);
}

exit(0);
?>
