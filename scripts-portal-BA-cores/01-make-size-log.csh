#!/bin/csh

set DIR = "_BA3A"
set LOG = "log.fullscale.targetsizes.$DIR.txt"

@ tw    = 1024
@ th    = 2048

@ TILEW  = 16
@ TTILEW =  3

echo $LOG

date >! $LOG

@ px = 0
@ py = 0

foreach i ( $DIR/fullscale/????/???/??/*.png)
  set info = `identify $i | sed -e 's/x/ /g'`
  set img  = `echo $i | sed -e 's_.*/__'`
  set dirp = `echo $i | sed -e 's/'$img'//'`
  set name = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TILEW'; nw=w/ratio; nh=int(h/ratio); print $1;}'`
  @   w    = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TILEW'; nw=w/ratio; nh=int(h/ratio); print w; }'`
  @   h    = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TILEW'; nw=w/ratio; nh=int(h/ratio); print h; }'`
  @   nw   = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TILEW'; nw=w/ratio; nh=int(h/ratio); print nw;}'`
  @   nh   = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TILEW'; nw=w/ratio; nh=int(h/ratio); print nh;}'`
  @   tnw  = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TTILEW'; nw=w/ratio; nh=int(h/ratio); print nw; }'`
  @   tnh  = `echo $info | awk '{w=$3; h=$4; ratio=w/'$TTILEW'; nw=w/ratio; nh=int(h/ratio); print nh; }'`
  if ( ( $py + $nh ) > $th ) then
    @ px = $px + $TILEW
    @ py = 0
  endif
  @ swir = `echo $img | grep SWIR | wc -l`
  set swir = `echo $img | sed -e 's/IR.*/IR/' -e 's/.*SW/SW/' -e 's/.*VN/VN/'`
  set imgpath = `echo $dirp | sed -e's/.*fullscale.//' -e 's_/_,_g'`
  #echo $imgpath " " $img " : " $swir " : " $w "x" $h " -> " $nw "x" $nh " @ " $px "x" $py
  echo $imgpath""$img","$swir","$w","$h","$nw","$nh","$tnw","$tnh
  @ py = $py + $nh
end
