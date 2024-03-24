<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $uploadDir = $_SERVER['UPLOAD_DIR']; // Directory where files will be uploaded
   // echo " ". $uploadDir;
    $uploadFile = $uploadDir . basename($_FILES['userfile']['name']);
   // echo " ". $uploadFile;
  
    if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadFile)) {
        echo "File is valid, and was successfully uploaded.\n";
    } else {
        echo "Possible file upload attack!\n";
    }
}
?>