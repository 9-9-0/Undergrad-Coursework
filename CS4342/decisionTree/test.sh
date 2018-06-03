#!/bin/bash
chr() {
	[ "$1" -lt 256 ] || return 1
	printf "\\$(printf '%03o' "$1")"
}

ord() {
	LC_CTYPE=C printf '%d' "'$1"
}

correct=0
incorrect=0
for i in $(cat ../example-data/bigDataCleaned.csv); do
	dtOut=$(./decisionTree $(echo $i | tr ',' ' ' | cut -d ' ' -f-129))
	actual=$(echo $i | cut -d ',' -f129 | tr -d '\r')
	if [ "$actual" = "$dtOut" ]; then
		correct=$(($correct+1))
	else
		incorrect=$(($incorrect+1))
	fi
	echo $i
done

echo "$correct correct predictions"
echo "$incorrect incorrect predictions"
