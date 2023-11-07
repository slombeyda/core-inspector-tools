#!/bin/csh

@ i =  0

while ( $i < 9 )
  ./binaryDataToPGM -width 320 -height 4229 -band $i < ../GT1A_100Z-1_minerals.img > band.$i.pgm
  convert band.$i.pgm band.$i.png
  \rm -f band.$i.pgm
  @ i = $i + 1
end
