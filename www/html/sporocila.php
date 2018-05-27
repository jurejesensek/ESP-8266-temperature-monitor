<?php

	include_once("glava.php");

	$sql = "SELECT * FROM messages ORDER BY id DESC";

	$ret = $db->query($sql);
	while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
		echo "id: ". $row['id'] . "\n";
		echo ", Sporocilo: ". $row['message'] ."\n";
		echo ", Ustvarjeno: ". $row['created_at'] ."<br>\n\n";
	}
	echo "Operation done successfully\n";

	include_once("noga.php");
