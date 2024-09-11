<?php

date_default_timezone_set("Asia/Kolkata");


if ($_SERVER["REQUEST_METHOD"] == "POST") {
    
    if (isset($_POST['CHIPID']) && isset($_POST['mpudata'])) {
        
        $macid = $_POST['CHIPID'];
        $data = $_POST['mpudata'];

       
        $fileName = "MpuData_" . $macid . "_" . date("d-m-y") . ".txt";

       
        $data = str_replace("/", date("H:i:s") . ',', $data);

        
        $fileStatus = file_put_contents($fileName, $data, FILE_APPEND | LOCK_EX);

       
        if ($fileStatus !== false) {
            echo "Data written successfully to file: " . $fileName;
        } else {
            echo "Failed to write data to the file.";
        }
    } else {
       
        echo "Error: Required fields 'CHIPID' or 'mpudata' are missing in the POST request.";
    }
} else {
   
    echo "Error: Invalid request method.";
}

?>
