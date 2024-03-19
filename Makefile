all: viewBinaryData binaryFloatsToPGM binaryDataToPGM convertTextFloatsToBase64

viewBinaryData: viewBinaryData.cpp
	c++ -O3 -o viewBinaryData viewBinaryData.cpp

binaryFloatsToPGM: binaryFloatsToPGM.cpp
	c++ -O3 -o binaryFloatsToPGM binaryFloatsToPGM.cpp

binaryDataToPGM: binaryDataToPGM.cpp
	c++ -O3 -o binaryDataToPGM binaryDataToPGM.cpp

convertTextFloatsToBase64: convertTextFloatsToBase64.cpp utils/Base64Encoder.o
	g++ -O3 -o convertTextFloatsToBase64 utils/Base64Encoder.o convertTextFloatsToBase64.cpp

utils/Base64Encoder.o: utils/Base64Encoder.cpp utils/Base64Encoder.h
	pushd utils; g++ -O3 -c Base64Encoder.cpp; popd;

clean:
	\rm -rf binaryFloatsToPGM viewBinaryData binaryDataToPGM band.*.png
