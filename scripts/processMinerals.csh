#!/usr/bin/csh

@ count = 0

foreach borehole ( GT1 GT2 GT3)
  pushd /home/santiago/data/$borehole
  foreach img ( C5701A-00????-??_????_??_??_??_??_??/*SWIRcalibminls_simple )

     set imghrd = $img.hdr
     set dirname =  `echo $img | sed -e 's_/.*__'`
     set filename = `echo $img | sed -e 's_.*/__'`
     set basedir = `echo $dirname | awk -F '-' '{ print $1; }'`
     set subdir0      = `echo $dirname | awk -F '-' '{ print $2; }'`
     set subdir1   = `echo $dirname | awk -F '-' '{ print $3; }' | sed -e 's/_.*//'`

     @ subdir0index = `echo $subdir0 | sed -e 's/Z//' -e 's/M//'`
     @ subdir1index = `echo $subdir1`

     @ rows = `grep samples  $imghrd | awk -F '=' '{ print $2; }'`
     @ cols = `grep lines    $imghrd | awk -F '=' '{ print $2 ;}'`
     @ mins = `grep bands    $imghrd | awk -F '=' '{ print $2; }'`

     	printf '%i,' $count
     	printf '"%s","%s","%s",' $borehole $dirname $filename
     	printf '"%s","%s","%s",' $basedir $subdir0 $subdir1
     	printf '%i,%i,'          $subdir0index $subdir1index
     	printf '%i,%i,%i\n' $rows $cols $mins

        @ w = $rows / 16
        @ h = $cols / 16

        set newfilename = `printf "%s_%04i_%02i"  $borehole $subdir0index $subdir1index`
        echo $newfilename.tmb.png ':' $w'x'$h
     @ count = $count + 1
  end

  popd

end
