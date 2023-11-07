# core-inspector-tools
# by santiago lombeyda

general tools for dealing with binary data files from oman core scans, and reducing their size



TOOLS
========================================



viewBinaryData.cpp
----------------------------------------
print out binary as bytes and as text

binaryFloatsToPGM.cpp
----------------------------------------
convert floats in range to grayscale 0..255

binaryDataToPGM.cpp
----------------------------------------
read int on/off (0 or >0), accumulate it into 16x16 buckets, and write out as grayscale pgm

