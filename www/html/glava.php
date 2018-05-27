<?php
  class MyDB extends SQLite3 {
      function __construct() {
         $this->open('../db/mqtt.db');
      }
   }
   $db = new MyDB();
   if(!$db) {
      echo $db->lastErrorMsg();
   } else if(isset($showMessage)) {
      echo "Opened database successfully\n";
   }
