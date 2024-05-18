#!/usr/bin/csh

set DATA_DIR = '/export/data11/OmanDP_processed/GT_dataset_archive'
set DEST_DIR = '/export/data11/OmanDP_portal_data'

set BIN_DIR  = '/home/santiago/src/core-inspector-tools'

set REF_DIR  = '/export/data11/OmanDP_portal_data/summaries'
set MIN_REF_FILE = $REF_DIR'/RGREENBERGER.MINERALS-BORELONG-META.csv'

@ count = 0

@ reducefactorRAW   =   1
@ reducefactorHIGH  =   2
@ reducefactorLARGE =   4
@ reducefactorBASE  =  16
@ reducefactorSMALL =  64
@ reducefactorTMB   =  96
@ reducefactorMINI  = 106
@ reducefactorNANO  = 160

@ VERBOSE = 0
@ DRYRUN  = 0
@ LOG     = 1
@ MINMAX  = 0

foreach borehole ( GT1A GT2A GT3A )

  if ( $MINMAX > 0 ) then
    echo "MINERAL,MIN,MAX" >! $DEST_DIR/$borehole/$borehole.MINERALS-META.csv
  endif

  if ( $DRYRUN > 0 ) then
    echo mkdir \-p $DEST_DIR/$borehole
  else
    mkdir -p $DEST_DIR/$borehole
  endif

  pushd $DATA_DIR/$borehole

  # [BASE]_minerals.hdr  [BASE]_minerals.img  [BASE]_SWIRcalib.hdr  [BASE]_SWIRcalib.img

  foreach img ( ${borehole}_*/*_minerals.img )

     set imgbase =   `echo $img | sed -e 's/.img$//'`
     set imghdr =    ${imgbase}.hdr
     set imgdir    = `echo $imgbase | sed -e 's_/.*__'`
     set filename  = `echo $imgbase | sed -e 's_.*/__'`

     set boreholedir = `echo $imgdir  | sed -e's/_/-/' | awk -F '-' '{ print $1; }'`
     set sectiondir  = `echo $imgdir  | sed -e's/_/-/' | awk -F '-' '{ print $2; }' | sed -e's/Z//'`
     set sectionZdir = `echo $imgdir  | sed -e's/_/-/' | awk -F '-' '{ print $2; }'`
     set piecedir    = `echo $imgdir  | sed -e's/_/-/' | awk -F '-' '{ print $3; }'`

     @ sectionindex  = `echo $sectiondir`
     @ pieceindex    = `echo $piecedir`

     set sectionZdir = `printf "%04iZ" $sectionindex`
     set piecedir    = `printf "%03i"  $pieceindex`

     @ rows = `grep -G '^samples'  $imghdr | awk -F '=' '{ print $2; }'`
     @ cols = `grep -G '^lines'    $imghdr | awk -F '=' '{ print $2; }'`
     @ mins = `grep -G '^bands'    $imghdr | awk -F '=' '{ print $2; }'`

     if ( $VERBOSE > 0 ) then
     echo 'imgbase:    '$imgbase
     echo 'img:        '$img
     echo 'hdr:        '$imghdr
     echo 'imgdir:     '$imgdir
     echo 'basedir:    '$boreholedir
     echo 'sectiondir: '$sectiondir
     echo 'piecedir:   '$piecedir
     echo 'nrows:      '$rows
     echo 'ncols:      '$cols
     echo 'nmins:      '$mins
     endif


     if ( $DRYRUN > 0 ) then
       echo mkdir \-p $DEST_DIR/$borehole/$sectionZdir
       echo mkdir \-p $DEST_DIR/$borehole/$sectionZdir/$piecedir
     else
       mkdir -p $DEST_DIR/$borehole/$sectionZdir
       mkdir -p $DEST_DIR/$borehole/$sectionZdir/$piecedir
     endif

     if ( $LOG > 0 ) then
     	printf '%i,' $count
     	printf '"%s","%s","%s",'      $borehole $imgdir $filename
     	printf '"%s","%s","%s","%s",' $boreholedir $sectiondir $sectionZdir $piecedir
     	printf '%i,%i,'               $sectionindex $pieceindex
     	printf '%i,%i,%i\n'           $rows $cols $mins
     endif

     #foreach reducefactor ( $reducefactorBASE $reducefactorSMALL $reducefactorTMB $reducefactorMINI $reducefactorNANO )
     #foreach reducefactor ( $reducefactorRAW $reducefactorLARGE )
     foreach reducefactor ( $reducefactorRAW )

       @ w = $rows / $reducefactor
       @ h = $cols / $reducefactor

       @ rf = $reducefactor

       @ n = 0


       @ WRITEJSON = 1
       if ( $reducefactor < 16 ) then
          @ WRITEJSON = 0
       endif

       @ ABUNDANCE = 0
       @ PRESENCE  = 0
       @ ABUNDANCE_LOCAL  = 0
       @ ABUNDANCE_GLOBAL = 0

       if ( $reducefactor == 1 ) then
          @ PRESENCE  = 0
          @ ABUNDANCE = 1
          @   ABUNDANCE_LOCAL  = 1
          @   ABUNDANCE_GLOBAL = 1
       endif

       # PA: PRESENCE/ABSCENSE  vs AB: ABUNDANCE
       while ( $n < $mins )
          set mindir      = `printf "%02i" $n`
          set productbase = `printf "%s_%s_%s_%s.factor_1to%03i" $borehole $sectionZdir $piecedir $mindir $rf`
          set json        = ${productbase}'.json'
          set png         = ${productbase}'.png'
          set alpng       = ${productbase}'.abundance.local.png'
          set agpng       = ${productbase}'.abundance.global.png'
          set pgm         = ${productbase}'.TMP.pgm'
          set alpngbase   = ${productbase}'.abundance.local'
          set agpngbase   = ${productbase}'.abundance.global'

          if ( $DRYRUN > 0 ) then
            echo mkdir \-p $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir
          else
            mkdir -p $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir
          endif

          if ( $DRYRUN > 0 ) then
              echo "+ " $BIN_DIR"/binaryDataToPGM " $mindir " / " $productbase "[.png|.json]"
          else
              # MINMAX
              if ( $MINMAX > 0 ) then
                    $BIN_DIR/binaryDataToPGM \
                    				-width $rows -height $cols -band $n \
                    				-float -factor $reducefactor \
                    				-minmaxcsv -quiet \
                    				< $img \
                    				>> $DEST_DIR/$borehole/$borehole.MINERALS-META.csv
      	      endif

              # JSON: PRESENCE/ABSENCE x REDUCEFACTOR
              if ( $WRITEJSON > 0 ) then
                    $BIN_DIR/binaryDataToPGM \
                    				-width $rows -height $cols -band $n \
                    				-onoff -factor $reducefactor \
                    				-json -quiet \
                    				< $img \
                    				> $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$json
      	      endif

              foreach e ( 0 4 12 )
              #foreach e ( 0 )

              set enhancement = ""
              set e_ext = ""

              if ( $e > 0 ) then
                  set enhancement = "-enhanced $e"
                  set e_ext       = ".e$e"
              endif

              # PNG: ABUNDANCE(local) x REDUCEFACTOR
              if ( $ABUNDANCE_LOCAL > 0 ) then
                $BIN_DIR/binaryDataToPGM \
                    				-width $rows -height $cols -band $n \
                    				-float -factor $reducefactor \
                                                -databounds 0.0 1.0 \
                    				-pgm -quiet \
                    				$enhancement \
                    				< $img \
                    				> $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm

                echo "." $alpngbase$e_ext.png
                convert $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm \
                    		      $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$alpngbase$e_ext.png
                \rm -f  $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm
      	      endif


              # PNG: ABUNDANCE(global) x REDUCEFACTOR
              if ( $ABUNDANCE_GLOBAL > 0 ) then
                #set minv = `cat $DEST_DIR/$borehole/$borehole.MINERALS-BORELONG-META.csv | awk -F ',' '{ if ( NR>1 && $1=='$n' ) { print $2 } }'`
                #set maxv = `cat $DEST_DIR/$borehole/$borehole.MINERALS-BORELONG-META.csv | awk -F ',' '{ if ( NR>1 && $1=='$n' ) { print $3 } }'`
                set minv = `cat $MIN_REF_FILE | awk -F ',' '{ if ( NR>1 && $1=='$n' ) { print $2 } }'`
                set maxv = `cat $MIN_REF_FILE | awk -F ',' '{ if ( NR>1 && $1=='$n' ) { print $3 } }'`

                $BIN_DIR/binaryDataToPGM \
                    				-width $rows -height $cols -band $n \
                    				-float -factor $reducefactor \
                                                -databounds $minv $maxv \
                    				-pgm -quiet \
                    				$enhancement \
                    				< $img \
                    				> $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm

                echo "." $agpngbase$e_ext.png
                convert $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm \
                    		      $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$agpngbase$e_ext.png
                \rm -f  $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm
      	      endif

              end

              # PNG: PRESENCE x REDUCEFACTOR
              if ( $PRESENCE > 0 ) then
                    $BIN_DIR/binaryDataToPGM \
                    				-width $rows -height $cols -band $n \
                    				-onoff -factor $reducefactor \
                    				-pgm -quiet \
                    				< $img \
                    				> $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm
                convert $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm \
                    		      $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$png
                \rm -f  $DEST_DIR/$borehole/$sectionZdir/$piecedir/$mindir/$pgm
                echo "." $png
      	      endif



          endif

          @ n = $n + 1

       end

     end

     @ count = $count + 1

  end

  popd

end
