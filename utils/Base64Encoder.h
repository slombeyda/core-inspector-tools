//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  //                                                encode to base 64
//  //  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  //                                           by santiago v lombeyda
//  //                                            cacr-caltech (c) 2005
//  //  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

#ifndef SVL_ENCODE_MIME_64
#define SVL_ENCODE_MIME_64

class Base64Encoder {
  public:
   static void encode(unsigned char *in3Bytes, unsigned char *out4Bytes);
   static int  encode(int n, unsigned char *inBytes, unsigned char *outBytes);
   static int  runEncode(int n, unsigned char *inBytes, unsigned char *outBytes, bool flushBuffer=false);

   static void runEncodeAndWrite(FILE *fptr, unsigned char *inBytes, int n, bool flushBuffer=false);
   static void runEncodeAndWriteInt(FILE *fptr, int i, bool flushBuffer=false);
   static void runEncodeAndWriteFlush(FILE *fptr);

   static void flipBytes(int size, int n, unsigned char *data);
   static bool isBigEndian();
   static bool isLittleEndian();

  private:
   static unsigned char encodedBuffer[256];
};

#endif
