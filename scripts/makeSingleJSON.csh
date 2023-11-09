#!/usr/bin/csh

@ kernel = 16
set k = ""

if ( $1 == "") then
  @ kernel = 16
  set k = ""
else
  @ kernel = $1
  set k = ".k$kernel"
endif

foreach f ( GT1 GT2 )
  @ i = 0
  while ( $i < 9 )
    echo $f.0$i$k.json
    echo '{'                            >! $f.0$i$k.json
    echo '"data": ['                    >> $f.0$i$k.json

    @ n = 0
    foreach j ( $f/*/*/*/*0$i$k.json )
         if ( $n > 0 ) then
           echo ','                     >> $f.0$i$k.json
         endif
         echo '{'                       >> $f.0$i$k.json
         echo '  "filename":"'$j'",'    >> $f.0$i$k.json
         echo '  "image":'              >> $f.0$i$k.json
         cat $j                         >> $f.0$i$k.json
         echo '}'                       >> $f.0$i$k.json
         @ n = $n + 1
    end

    echo ']'                            >> $f.0$i$k.json
    echo '}'                            >> $f.0$i$k.json

    @ i = $i + 1
  end
end
