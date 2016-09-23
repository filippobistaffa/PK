#!/usr/bin/env bash

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 INPUTFILE"
	exit
fi

if [ ! -f "$1" ]
then
	echo "$1 not found"
	exit
fi

N=`sed '1q;d' $1`	# Number of agents
K=`sed '2q;d' $1`	# Maximum cardinality

tmp=`mktemp`
echo "#define N $N" > $tmp
echo "#define K $K" >> $tmp
echo "#define INPUTFILE \"$1\"" >> $tmp

if [ ! -f instance.h ]
then
	mv $tmp "instance.h"
else
	md5a=`md5sum instance.h | cut -d\  -f 1`
	md5b=`md5sum $tmp | cut -d\  -f 1`

	if [ $md5a != $md5b ]
	then
		mv $tmp "instance.h"
	else
		rm $tmp
	fi
fi

make -j
if [[ $? == 0 ]]
then
	./pc
fi
