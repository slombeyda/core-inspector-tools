#!/bin/csh

foreach c ( 1 3 )
  mkdir -p "BA"$c"A/downscaled"
  mkdir -p "BA"$c"A/punchcard"
  foreach m ( 0 1 2 3 4 5 6 7 8 )

     echo "+ BA"$c":"$m

     set SOURCE = "-csv logs/BA${c}A_fullscale_sizes.csv -folder BA${c}A/fullscale -mineral $m"
     echo "  : "$SOURCE

     src/compactFullscaletoSingle.py $SOURCE \
				      -tilewidth 3 \
				      -width 256 -height 512 \
				      -o BA${c}A/punchcard/${m}.png

     src/compactFullscaletoSingle.py $SOURCE \
				      -tilewidth 16 \
				      -width 1024 -height 2048 \
				      -o BA${c}A/downscaled/${m}.png

  end

end
