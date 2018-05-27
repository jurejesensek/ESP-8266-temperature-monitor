#!/bin/sh
cd $(dirname $0)
mosquitto_sub -R -u bso -P vaje -t dev/test | while read msg
do
  echo $msg # replace with your task of choice
  php /var/www/html/saveToDB.php "$msg"
done
