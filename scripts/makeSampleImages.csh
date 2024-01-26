#!/bin/csh

set src = "../../data/sample/GT1A_100Z-1/GT1A_100Z-1_minerals.img"
#./viewBinaryData -float < $src | awk 'BEGIN{m=0}{if ($1>m) m=$1;}END{print m}'

@ samples = 320
@ lines   = 4228
@ bands   = 9

@ rows = $samples
@ cols = $lines
@ reducefactor = 1

set img = $src
set base = tmp/GT1A_100Z-1_minerals

@ n = 0
while ( $n < $bands )

  ./binaryDataToPGM \
                                -width $rows -height $cols -band $n \
                                -onoff -factor $reducefactor \
                                -pgm -quiet \
                                < $img \
                                > ${base}_0$n.onoff.pgm

  convert ${base}_0$n.onoff.pgm ${base}_0$n.onoff.png
  \rm -f ${base}_0$n.onoff.pgm

  ./binaryDataToPGM \
                                -width $rows -height $cols -band $n \
                                -float -factor $reducefactor \
                                -pgm -quiet \
                                < $img \
                                > ${base}_0$n.f.pgm

  convert ${base}_0$n.f.pgm ${base}_0$n.f.png
  \rm -f ${base}_0$n.f.pgm

  ./binaryDataToPGM \
                                -width $rows -height $cols -band $n \
                                -float -factor $reducefactor \
                                -pgm -quiet \
                                < $img \
                                > ${base}_0$n.f.pgm

  convert ${base}_0$n.f.pgm ${base}_0$n.f.png
  \rm  -f ${base}_0$n.f.pgm

  foreach e ( 2 6 12 )
    ./binaryDataToPGM \
                                -width $rows -height $cols -band $n \
                                -float -factor $reducefactor \
                                -pgm -quiet -enhanced ${e} \
                                < $img \
                                > ${base}_0$n.f_e${e}.pgm

    convert ${base}_0$n.f_e${e}.pgm ${base}_0$n.f_e${e}.png
    \rm  -f ${base}_0$n.f_e${e}.pgm
  end

  @ n = $n + 1
end
