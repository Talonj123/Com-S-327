#!/bin/ksh
ls *.c >amit.temp

while read line

do
echo $line
new=${line}pp
echo $new
mv $line $new > /dev/null
echo "Renamed all c files to cpp files"
done < amit.temp
