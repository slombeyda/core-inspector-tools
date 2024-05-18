#!/bin/csh


@ MIN_HYDRATION_VALUE = 0
@ MAX_HYDRATION_VALUE = 2

set DATA_DIR = "/export/data11/OmanDP_processed/GT_dataset_archive/Hydration"
set BIN_DIR  = "/home/santiago/src/core-inspector-tools"
set OUT_DATA_DIR = "/export/data11/OmanDP_portal_data"

foreach BORE_HOLE ( GT1A GT2A GT3A )
  echo $BORE_HOLE
  set BASE_DIR = $DATA_DIR/${BORE_HOLE}"ThruZero"
  foreach ROCK_SECTION_PATH ( $BASE_DIR/$BORE_HOLE* )

    set ROCK_SECTION_AND_PIECE_DIR = `echo $ROCK_SECTION_PATH | sed -e's/.*'$BORE_HOLE'_//'`
    set ROCK_SECTION               = `echo $ROCK_SECTION_AND_PIECE_DIR | awk -F'-' '{print int($1)"Z"}'`
    set ROCK_PIECE                 = `echo $ROCK_SECTION_AND_PIECE_DIR | awk -F'-' '{print int($2)}'`
    @   ROCK_SECTION_NUMBER        = `echo $ROCK_SECTION | sed -e 's/[^0-9]//g'`
    set ROCK_SECTION_LETTER        = `echo $ROCK_SECTION | sed -e 's/[0-9]//g'`
    set ROCK_SECTION_DIR           = `printf "%04i%c" $ROCK_SECTION_NUMBER $ROCK_SECTION_LETTER`
    set ROCK_PIECE_DIR             = `printf "%03i"   $ROCK_PIECE`

    echo "+" $BORE_HOLE $ROCK_SECTION $ROCK_PIECE

    set  HYDRATION_FILE = "$ROCK_SECTION_PATH/${BORE_HOLE}_${ROCK_SECTION_AND_PIECE_DIR}_Hydration.img"
    set  HYDRATION_INFO = "$ROCK_SECTION_PATH/${BORE_HOLE}_${ROCK_SECTION_AND_PIECE_DIR}_Hydration.hdr"

    echo "> RGB "     $OUT_DATA_DIR $BORE_HOLE $ROCK_SECTION_DIR $ROCK_PIECE_DIR
    set TARGET_DIR = "$OUT_DATA_DIR/$BORE_HOLE/$ROCK_SECTION_DIR/$ROCK_PIECE_DIR"

    @ w        = `grep "^samples" $HYDRATION_INFO | awk -F'=' '{print int($2)}'`
    @ h        = `grep "^lines"   $HYDRATION_INFO | awk -F'=' '{print int($2)}'`

    echo "> RGB " $OUT_DATA_DIR $BORE_HOLE $ROCK_SECTION_DIR $ROCK_PIECE_DIR


    set OUT_RGB_DIR        = "$TARGET_DIR/hydration"
    set OUT_RGB_FILEBASE   = "${BORE_HOLE}_${ROCK_SECTION_DIR}_${ROCK_PIECE_DIR}_hydration"

    echo "w h"
    echo $w $h

    mkdir -p $OUT_RGB_DIR

    ls -la $HYDRATION_FILE

    echo $OUT_RGB_DIR/$OUT_RGB_FILEBASE.png

    $BIN_DIR/binaryFloatsTo16bitPGM -width $w -height $h -bounds 0 16 \
		< $HYDRATION_FILE \
		> ~/$OUT_RGB_FILEBASE.ppm

    convert ~/$OUT_RGB_FILEBASE.ppm $OUT_RGB_DIR/$OUT_RGB_FILEBASE.png
    \rm -r ~/$OUT_RGB_FILEBASE.ppm

  end
end

