# core-inspector-tools
# by santiago lombeyda (2013)

general tools for dealing with binary data files from oman core scans, and reducing their size



TOOLS
========================================
use -h as parameter in command line tool for usage of individual tool


viewBinaryData (c++)
----------------------------------------
print out binary as bytes and as text

binaryFloatsToPGM (c++)
----------------------------------------
convert floats in range to grayscale 0..255

binaryDataToPGM (c++)
----------------------------------------
read int on/off (0 or >0), accumulate it into 16x16 buckets, and write out as grayscale pgm

