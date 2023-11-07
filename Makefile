viewBinaryData: viewBinaryData.cpp
	c++ -O3 -o viewBinaryData viewBinaryData.cpp

binaryFloatsToPGM: binaryFloatsToPGM.cpp
	c++ -O3 -o binaryFloatsToPGM binaryFloatsToPGM.cpp

clean:
	\rm -rf binaryFloatsToPGM viewBinaryData
