#!/bin/csh

set MAX_LENGTH = "400.0"
set CORE = "3"

set TOTAL_LENGTH = `cat logs/BA${CORE}A_fullscale_sizes.csv | awk -F',' \
	'BEGIN{ L = 0 }\
	      { if ($3=="01") { L = L + ($7 / $6); }}\
		  END { print L}'`


printf "{"
cat logs/BA1A_fullscale_sizes.csv | awk -F',' \
	'BEGIN{ TL = '$TOTAL_LENGTH'; al=0; }\
	      { if ($3=="01") { \
		  		if (al>0) printf(","); \
		  		l=($7/$6/TL)*'$MAX_LENGTH'; \
		  		printf("\"%i_%i\":{\"topDepth\":%l.15f,\"length\":%.15lf}",$1,$2,al,l); \
				al=al+l; }}\
	 END  { }'
printf "}"