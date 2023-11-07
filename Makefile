all: viewBinaryData binaryFloatsToPGM binaryDataToPGM


viewBinaryData: viewBinaryData.cpp
	c++ -O3 -o viewBinaryData viewBinaryData.cpp

binaryFloatsToPGM: binaryFloatsToPGM.cpp
	c++ -O3 -o binaryFloatsToPGM binaryFloatsToPGM.cpp

binaryDataToPGM: binaryDataToPGM.cpp
	c++ -O3 -o binaryDataToPGM binaryDataToPGM.cpp

clean:
	\rm -rf binaryFloatsToPGM viewBinaryData binaryDataToPGM band.*.png
