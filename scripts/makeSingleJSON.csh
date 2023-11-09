#!/usr/bin/csh

@ kernel = 16
if ( $1 == "") then
  @ kernel = 16
else
  @ kernel = $1
endif

foreach f ( GT1 GT2 )
  @ i = 0
  while ( $i < 9 )
    echo $f.0$i.k$kernel.json
    echo '{'                        >! $f.0$i.k$kernel.json
    echo '"data": ['                >> $f.0$i.k$kernel.json

      foreach j ( $f/*/*/*/*0$i.k$kernel.json )
         echo '{'                       >> $f.0$i.k$kernel.json
         echo '  "filename":"'$j'",'    >> $f.0$i.k$kernel.json
         echo '  "image":'              >> $f.0$i.k$kernel.json
         cat $j                         >> $f.0$i.k$kernel.json
         echo '},'                      >> $f.0$i.k$kernel.json
      end

      echo ']'                >> $f.0$i.k$kernel.json
      echo '}'                >> $f.0$i.k$kernel.json

      @ i = $i + 1
   end
end
