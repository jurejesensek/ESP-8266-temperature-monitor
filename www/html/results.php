<?php

	include_once("glava.php");

	$sql = "SELECT * FROM results ORDER BY id DESC";

	$ret = $db->query($sql);
	while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
		echo "id: ". $row['id'] . "\n";
		echo ", naprava: ". $row['sender'] ."\n";
		echo ", temperatura: ". $row['temperature'] ."\n";
		echo ", senzor: ". $row['sensor'] ."\n";
		echo ", Ustvarjeno: ". $row['created_at'] ."<br>\n\n";
	}
	echo "Operation done successfully\n";

	include_once("noga.php");
