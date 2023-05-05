<?php
    // log in vao database
    include("config.php");
    // doc user input
    //$br= $_POST["brightness"];
    $id = $_POST["den"]; ;
    $red = $_POST["r"];
    $green = $_POST["g"];
    $yellow = $_POST["y"];
    // update lai database
    $sql = "update control set red =$red, yellow=$yellow,green=$green where id=$id";
    mysqli_query($conn, $sql);
    mysqli_close($conn);

?>



