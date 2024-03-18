foreach i ( GT?/*/*/*/*.pgm )
  set j = `echo $i | sed -e 's/pgm$/png/'`
  convert $i $j
end
