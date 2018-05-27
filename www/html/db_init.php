<?php
die("locked");
include_once("glava.php");
      $sql =<<<EOF
      CREATE TABLE results
      (id INTEGER PRIMARY KEY AUTOINCREMENT,
      sender CHAR(31) NOT NULL,
      temperature  CHAR(15)   NOT NULL,
      sensor CHAR(31) DEFAULT "",
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP);

	CREATE TABLE messages
	(id INTEGER PRIMARY KEY AUTOINCREMENT,
	message  CHAR(63)   NOT NULL,
	created_at DATETIME DEFAULT CURRENT_TIMESTAMP);

EOF;

   $ret = $db->exec($sql);
   if(!$ret){
      echo $db->lastErrorMsg();
   } else {
      echo "Table created successfully\n";
   }

	include_once("noga.php");

?>
