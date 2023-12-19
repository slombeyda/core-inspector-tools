#!/bin/csh


@ MIN_SPECTRA_VALUE = 0
@ MAX_SPECTRA_VALUE = 2

set DATA_DIR = "/export/data11/OmanDP_processed/GT_dataset_archive"
set BIN_DIR  = "/home/santiago/src/core-inspector-tools"

foreach BORE_HOLE ( GT1A )

    set BASE_DIR = $DATA_DIR/$BORE_HOLE

    @ W_RF =    16
    @ S_RF =     3
    @ H_RF =    16
    @ NSLICES = $H_RF * 8

    #if (DEBUG) # confirm exisitence of file
    #ls -la $BASE_DIR/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img

    echo \
    $BIN_DIR/binaryFloatsToPGM -jsonfloats \
	        -bounds $MIN_SPECTRA_VALUE $MAX_SPECTRA_VALUE \
		-reduce $W_RF $S_RF $H_RF \
	  		$BASE_DIR/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img \
	  	-outputinpieces -slicesperpiece $NSLICES \
	  	-fileoutprefix ~/sample-GT1A_100Z-1_SPECTRA

    $BIN_DIR/binaryFloatsToPGM -jsonfloats \
	        -bounds $MIN_SPECTRA_VALUE $MAX_SPECTRA_VALUE \
		-reduce $W_RF $S_RF $H_RF \
	  		$BASE_DIR/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img \
	  	-outputinpieces -slicesperpiece $NSLICES \
	  	-fileoutprefix ~/sample-GT1A_100Z-1_SPECTRA

end
