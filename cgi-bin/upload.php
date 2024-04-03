<?php
    $uploadDir = $_SERVER['UPLOAD_DIR']; // Directory where files will be uploaded
    echo " ". $uploadDir . "\n";
    $uploadFile = $uploadDir . "/" . basename($_FILES['file']['name']);
    echo " ". $uploadFile . "\n";
  
    if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadFile)) {
        echo "File is valid, and was successfully uploaded.\n";
    } else {
        echo "Possible file upload attack!\n";
    }
?>