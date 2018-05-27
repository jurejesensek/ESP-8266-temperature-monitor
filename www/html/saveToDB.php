<?php
	ini_set('display_errors', 1);
	ini_set('display_startup_errors', 1);
	error_reporting(E_ALL);

	if(! isset($argv[1])) die("Brez podatkov");

	$msg = $argv[1];
	echo "Prebrano: " . $argv[1] . "\n";
	include_once("glava.php");

	$sql = "INSERT INTO messages (message) VALUES ('$msg')";

	$ret = $db->exec($sql);
	if(!$ret) {
		echo $db->lastErrorMsg();
	} else {
		echo "Records saved to messages\n";
	}

	$arr = explode(":", $msg);
	if(count($arr) > 1){

		$naprava = $arr[0];
		$temp = $arr[1];
		$senzor = isset($arr[2]) ? $arr[2] : "";

		$sql = "INSERT INTO results (sender, temperature, sensor) VALUES ('$naprava', '$temp', '$senzor')";

	        $ret = $db->exec($sql);
	        if(!$ret) {
	                echo $db->lastErrorMsg();
	        } else {
	                echo "Record procesed to results\n";
	        }

	}

	include_once("noga.php");

?>
