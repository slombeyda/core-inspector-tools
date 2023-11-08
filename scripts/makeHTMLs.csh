#!/bin/csh

set currentdir = `pwd`
set scriptdir = `echo $0 | sed -e 's_\(.*\)/.*_\1_'`

set bindir =  $currentdir/$scriptdir

foreach d ( GT? )
  @ i = 0
  while ( $i < 9 )
    set htmlpage = "$d.0$i.html"
    cp $bindir/index.HEADER.html $htmlpage
    ls $d/*/*/*/*$i.png | awk '{print "  <img src="$0">"}' >>  $htmlpage
    cat $bindir/index.FOOTER.html >> $htmlpage
    @ i = $i + 1
   end
end
