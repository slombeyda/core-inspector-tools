#!/bin/csh


@ MIN_SPECTRA_VALUE = 0
@ MAX_SPECTRA_VALUE = 2

set DATA_DIR = "/export/data11/OmanDP_processed/GT_dataset_archive"
set BIN_DIR  = "/home/santiago/src/core-inspector-tools"
set OUT_DATA_DIR = "/export/data11/OmanDP_portal_data"

# LOW RES, CURVES GET FLATTENED, BUT SMALL SIZE
@ W_RF =    16
@ S_RF =     3
@ H_RF =    16

# MED RES
@ W_RF =     4
@ S_RF =     3
@ H_RF =     4

# HIGH RES
@ W_RF =     4
@ S_RF =     1
@ H_RF =     4

@ NSLICES = $H_RF * 4


#foreach BORE_HOLE ( GT1A GT2A GT3A )
foreach BORE_HOLE ( GT3A )
  echo $BORE_HOLE
  set BASE_DIR = $DATA_DIR/$BORE_HOLE
  foreach ROCK_SECTION_PATH ( $BASE_DIR/$BORE_HOLE* )

    set ROCK_SECTION_AND_PIECE_DIR = `echo $ROCK_SECTION_PATH | sed -e's/.*'$BORE_HOLE'_//'`
    set ROCK_SECTION               = `echo $ROCK_SECTION_AND_PIECE_DIR | awk -F'-' '{print $1}'`
    set ROCK_PIECE                 = `echo $ROCK_SECTION_AND_PIECE_DIR | awk -F'-' '{print $2}'`
    @   ROCK_SECTION_NUMBER        = `echo $ROCK_SECTION | sed -e 's/[^0-9]//g'`
    set ROCK_SECTION_LETTER        = `echo $ROCK_SECTION | sed -e 's/[0-9]//g'`
    set ROCK_SECTION_DIR           = `printf "%04i%c" $ROCK_SECTION_NUMBER $ROCK_SECTION_LETTER`
    set ROCK_PIECE_DIR             = `printf "%03i"   $ROCK_PIECE`

    echo "+" $BORE_HOLE $ROCK_SECTION $ROCK_PIECE

    set  SPECTRA_FILE = "$ROCK_SECTION_PATH/${BORE_HOLE}_${ROCK_SECTION_AND_PIECE_DIR}_SWIRcalib.img"
    set  SPECTRA_INFO = "$ROCK_SECTION_PATH/${BORE_HOLE}_${ROCK_SECTION_AND_PIECE_DIR}_SWIRcalib.hdr"

    #echo $OUT_DATA_DIR $BORE_HOLE $ROCK_SECTION_DIR $ROCK_PIECE_DIR
    set TARGET_DIR = "$OUT_DATA_DIR/$BORE_HOLE/$ROCK_SECTION_DIR/$ROCK_PIECE_DIR"

    @ w        = `grep "^samples" $SPECTRA_INFO | awk -F'=' '{print $2}'`
    @ nsamples = `grep "^bands" $SPECTRA_INFO | awk -F'=' '{print $2}'`
    @ h        = `grep "^lines" $SPECTRA_INFO | awk -F'=' '{print $2}'`


    set OUT_SPECTRA_REDUCE = "W${W_RF}_S${S_RF}_H${H_RF}-n{$NSLICES}"
    set OUT_SPECTRA_DIR    = "$TARGET_DIR/spectra/${OUT_SPECTRA_REDUCE}"
    set OUT_SPECTRA_PREFIX = "${BORE_HOLE}_${ROCK_SECTION_AND_PIECE_DIR}_${OUT_SPECTRA_REDUCE}"

    #ls -la $SPECTRA_FILE
    #echo $OUT_SPECTRA_DIR/
    #echo $OUT_SPECTRA_PREFIX


    #if (DEBUG) # confirm exisitence of file
    #ls -la $BASE_DIR/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img

    echo "> "$OUT_SPECTRA_DIR

    mkdir -p $OUT_SPECTRA_DIR

    $BIN_DIR/binaryFloatsToPGM -jsonfloats \
		-width $w -nsamples $nsamples -height $h \
	        -bounds $MIN_SPECTRA_VALUE $MAX_SPECTRA_VALUE \
		-reduce $W_RF $S_RF $H_RF \
	  		$SPECTRA_FILE \
	  	-outputinpieces -slicesperpiece $NSLICES \
	  	-fileoutprefix $OUT_SPECTRA_DIR/$OUT_SPECTRA_PREFIX
   echo "."
  end
end

