#!/usr/bin/csh

echo "BAND,MIN,MAX"

@ i = 0
while ( $i < 9 )
  cat GT?A.MINERALS-META.csv | awk -F',' \
    'BEGIN{ min=max=0; } \
     {\
	if ($1=='$i') { \
		if ($3!="inf" && $2<min ) { min=$2; } \
		if ($3!="inf" && $3>max ) { max=$3; } \
	} \
      } \
      END{ OFS=","; print '$i',min,max}'
  @ i = $i + 1
end
