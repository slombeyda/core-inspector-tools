#!/usr/bin/csh 

set BASE_DIR = "/export/data11/OmanDP_processed/GT_dataset_archive/GT1A/GT1A_100Z-3" 

echo "const MINERAL_BAND_NAMES = ["

tail -2 $BASE_DIR/GT1A_100Z-3_minerals.hdr | \
	sed -e 's/ *//g' -e 's/}$//' -e's/,$//' | \
	awk -F',' '{for (i=1; i<=NF; i++) { if (i==NF) ENDMARKER=""; else ENDMARKER=","; print "  \""$i"\""ENDMARKER; }}'

echo "];"
