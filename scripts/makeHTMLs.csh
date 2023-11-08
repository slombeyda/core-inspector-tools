#!/bin/csh

foreach d ( GT? )
  @ i = 0
  while ( $i < 9 )
    set htmlpage = "$d.0$i.html"
    cp index.HEADER.html $htmlpage
    ls $d/*/*/*/*$i.png | awk '{print "  <img src="$0">"}' >>  $htmlpage
    @ i = $i + 1
   end
end
