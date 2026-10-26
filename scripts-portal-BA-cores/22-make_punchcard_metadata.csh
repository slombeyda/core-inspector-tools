#!/bin/csh

set LOG="logs/BA1A_fullscale_sizes.csv"

if ($#argv == 1) then
    set LOG="logs/BA${argv[1]}A_fullscale_sizes.csv"
endif

printf '{"dimensions":[%i,%i],' 256 512

cat $LOG | awk -F',' 'BEGIN{n=0;l=0;}\
                           {if ($3=="03") {n++;l=l+$11;}}\
                      END  {printf("\"numTiles\":%i,\"totalHeight\":%i,",n,l)}'

printf '"tiles":['
cat $LOG | awk -F',' 'BEGIN{px=0; py=0; IW=256; IH=512; dw=3; n=0;} {if ($3=="03") {\
        w=$10; h=$11; \
        if (n>0) printf(","); \
        printf("{\"left\":%i,\"top\":%i,\"width\":%i,\"height\":%i}",px,py,w,h) \
        if (py+h<IH) py=py+h; else { px=px+dw; py=0; } \
        n++; }}'
printf ']}'
