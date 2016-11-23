#!/usr/bin/env bash

e=0.05					# Default epsilon = 0.05
red='\033[0;31m'			# Red
nc='\033[0m'				# No color
re='^[0-9]+$'				# Regular expression to detect natural numbers
rd='^[0-9]+\.?[0-9]*$'			# Regular expression to detect real numbers

usage() { echo -e "Usage: $0 -i <filename> [-e <epsilon>]\n-i\tInput filename\n-e\tEpsilon (optional, default e = 0.05)" 1>&2; exit 1; }

while getopts ":i:e" o; do
	case "${o}" in
	i)
		i=${OPTARG}
		if [ ! -f "$i" ]
		then
			echo -e "${red}Input file \"$i\" not found!${nc}\n" 1>&2
			usage
		fi
		;;
	e)
		e=${OPTARG}
		if ! [[ $e =~ $rd ]] ; then
			echo -e "${red}Epsilon must be a real number!${nc}\n" 1>&2
			usage
		fi
		;;
	\?)
		echo -e "${red}-$OPTARG is not a valid option!${nc}\n" 1>&2
		usage
		;;
	esac
done
shift $((OPTIND-1))

if [ -z "${i}" ]
then
	echo -e "${red}Missing one or more required options!${nc}\n" 1>&2
	usage
fi

N=`sed '1q;d' $i`	# Number of agents
K=`sed '2q;d' $i`	# Maximum cardinality

tmp=`mktemp`
echo "#define N $N" > $tmp
echo "#define K $K" >> $tmp
echo "#define EPSILON $e" >> $tmp
echo "#define CORES `grep -c ^processor /proc/cpuinfo`" >> $tmp

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
	bin=$0
	bin=${bin%???}
	$bin $i
fi
