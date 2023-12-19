#!/bin/csh

@ NSLICES = 32

echo PGMS

./binaryFloatsToPGM -pgm \
	  -bounds -0.182627 2.111866 \
		-reduce 4 3 4 \
	  ../../data/sample/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img \
	  -outputinpieces -slicesperpiece $NSLICES -fileoutprefix ~/Desktop/slices/pgms/GT1A_100Z-1_SPECTRA


echo JSON

./binaryFloatsToPGM -jsonfloats \
	  -bounds -0.182627 2.111866 \
		-reduce 4 3 4 \
	  ../../data/sample/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img \
	  -outputinpieces -slicesperpiece $NSLICES -fileoutprefix ~/Desktop/slices/json/GT1A_100Z-1_SPECTRA

exit

echo TEXTSHORTS

echo ./binaryFloatsToPGM -textshorts \
	  -bounds -0.182627 2.111866 \
	  ../../data/sample/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img \
	  -outputinpieces -slicesperpiece $NSLICES -fileoutprefix ~/Desktop/slices/shorts/GT1A_100Z-1_SPECTRA \
		>& /dev/null

echo TEXTFLOATS

echo ./binaryFloatsToPGM -textfloats \
	  -bounds -0.182627 2.111866 \
	  ../../data/sample/GT1A_100Z-1/GT1A_100Z-1_SWIRcalib.img \
	  -outputinpieces -slicesperpiece $NSLICES -fileoutprefix ~/Desktop/slices/floats/GT1A_100Z-1_SPECTRA \
		>& /dev/null
