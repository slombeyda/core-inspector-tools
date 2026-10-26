#!/bin/csh

set DIR = "_BA1A"

#echo "IMAGES_DB_"$DIR" = ["
ls $DIR/fullscale/0???/00?/0?/*.png | \
		sed -e 's_/_ _g' | \
		sed -e 's/IR.*png/IR/' | \
		sed -e 's/BA._.*SWIR/SWIR/' | \
		sed -e 's/BA._.*VNIR/VNIR/' | \
		sed -e 's/BA.A_.*SWIR/SWIR/' | \
		sed -e 's/BA.A_.*VNIR/VNIR/' | \
		awk '{printf("\"%s\",\"%s\",\"%s\",\"%s\"\n",$3,$4,$5,$6);}'
#		awk '{printf("  [\"%s\",\"%s\",\"%s\",\"%s\"]\n",$3,$4,$5,$6);}'
#print "];"
