#!/usr/bin/csh

set DATA_DIR = '/export/data11/OmanDP_processed'
set DEST_DIR = '/export/data11/OmanDP_portal_data'

set BIN_DIR  = '/home/santiago/src/core-inspector-tools'

@ count = 0

foreach borehole ( GT2 GT3 )

  mkdir -p $DEST_DIR/$borehole

  pushd $DATA_DIR/$borehole

  foreach img ( C?????-00????-??_????_??_??_??_??_??/*SWIRcalibminls_simple )

     set imghrd = $img.hdr
     set dirname =  `echo $img | sed -e 's_/.*__'`
     set filename = `echo $img | sed -e 's_.*/__'`
     set basedir = `echo $dirname | awk -F '-' '{ print $1; }'`
     set subdir0   = `echo $dirname | awk -F '-' '{ print $2; }'`
     set subdir0c  = `echo $subdir0 | sed -e 's/M$//' -e 's/Z$//' -e's/^0//'`
     set subdir1   = `echo $dirname | awk -F '-' '{ print $3; }' | sed -e 's/_.*//'`

     @ subdir0index = `echo $subdir0c`
     @ subdir1index = `echo $subdir1`

     @ rows = `grep samples  $imghrd | awk -F '=' '{ print $2; }'`
     @ cols = `grep lines    $imghrd | awk -F '=' '{ print $2 ;}'`
     @ mins = `grep bands    $imghrd | awk -F '=' '{ print $2; }'`

     mkdir -p $DEST_DIR/$borehole/$subdir0c
     mkdir -p $DEST_DIR/$borehole/$subdir0c/$subdir1

     	printf '%i,' $count
     	printf '"%s","%s","%s",'      $borehole $dirname $filename
     	printf '"%s","%s","%s","%s",' $basedir $subdir0 $subdir0c $subdir1
     	printf '%i,%i,'               $subdir0index $subdir1index
     	printf '%i,%i,%i\n'           $rows $cols $mins

        @ w = $rows / 16
        @ h = $cols / 16

        #set newfilename = `printf "%s_%04i_%02i"  $borehole $subdir0index $subdir1index`
        #echo $newfilename.tmb.png ':' $w'x'$h

        @ n = 0
        while ( $n < $mins )
          set mindir = `printf "%02i" $n`
          mkdir -p $DEST_DIR/$borehole/$subdir0c/$subdir1/$mindir
          set tmb = `printf "%s_%s_%s_%02i.pgm" $borehole $subdir0c $subdir1 $n`
          $BIN_DIR/binaryDataToPGM -width $rows -height $cols -band $n -quiet < $img > $DEST_DIR/$borehole/$subdir0c/$subdir1/$mindir/$tmb
          @ n = $n + 1
        end
     @ count = $count + 1
  end

  popd

end
