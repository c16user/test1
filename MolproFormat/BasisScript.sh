#!/bin/bash
cat main.cpp | grep '^.*export.*Molpro' | awk '{print $3}' > temp
var=`cat temp | sed 's/"\(.*\)"/\1/'`
echo $goodVar
sed -e '/^\s*$/d' -i $var
rm temp 
