#!/bin/csh

set LOG="logs/BA3A_fullscale_sizes.csv"

if ($#argv == 1) then
    set LOG="logs/BA${argv[1]}A_fullscale_sizes.csv"
endif

printf '{"dimensions":[%i,%i],' 1024 2048

cat $LOG | awk -F',' 'BEGIN{n=0;l=0;}\
                           {if ($3=="03") {n++;l=l+$9;}}\
                      END  {printf("\"numTiles\":%i,\"totalHeight\":%i,",n,l)}'

printf '"tiles":['
cat $LOG | awk -F',' 'BEGIN{px=0; py=0; IW=1024; IH=2048; dw=16; n=0;} {if ($3=="03") {\
        w=$8; h=$9; \
        if (n>0) printf(","); \
        printf("{\"left\":%i,\"top\":%i,\"width\":%i,\"height\":%i}",px,py,w,h) \
        if (py+h<IH) py=py+h; else { px=px+dw; py=0; } \
        n++; }}'
printf ']}'
