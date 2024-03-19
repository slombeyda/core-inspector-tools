#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "Base64Encoder.h"

//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                encode to base 64
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                           by santiago v lombeyda
//                                            cacr-caltech (c) 2004
//
//  base 64 encoders are very simple. all they do
//  is take 6 bits at a time and translate them into
//  an 8 bit char corresponding to letters A-Z a-z + /.
//  total 64 bit ascii characters. the only issue is
//  when the number of bits is not a multiple of 6. so,
//  you simple do it 3 binary characters at a time into
//  four chars, and if you do not have enough, you simply
//  stuff 8 or 16 0 bits (1 or 2 chars) to add up to the
//  wanted 24 bits that will become 32 bits (4 chars).
//
//  this implementation, takes advantage of the first and
//  last bits to quickly encode them using one of 2 lookup
//  char conversion tables. the middle two 6 bits are treated
//  the usual way, converting them into 2 zero bits and then
//  the lookup 6.
//
//  (is a memory lookup faster than 3 bit operations? maybe not)
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static const char* TABLE_BYTE_3_B64 ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ ";

static const char* TABLE_BYTE_0_B64 ="AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz0000111122223333444455556666777788889999++++//// ";

# define encodeCHARToBase64(c) (((c)<=25)?(c+'A'):(\
			        ((c)<=51)?(c+'a'-26):(\
			        ((c)<=61)?(c+'0'-52):(\
			        ((c)==61)?'+':'/'))))

unsigned char Base64Encoder::encodedBuffer[256];

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                     isBigEndian
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Base64Encoder::isBigEndian() {
# define IS_BIG_ENDIAN    2
# define IS_LITTLE_ENDIAN 1
  static int checkedEndianess=0;
  if (checkedEndianess==0) {
    const int i=1;
    char *c=(char *)&i;
    if (c[sizeof(int)-1]==0) checkedEndianess=IS_LITTLE_ENDIAN;
    else checkedEndianess=IS_BIG_ENDIAN;
  }
  return (checkedEndianess==IS_BIG_ENDIAN);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                  isLittleEndian
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Base64Encoder::isLittleEndian() {
  return !isBigEndian();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                   encodeToBase64
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Base64Encoder::encode(unsigned char *in3Bytes, unsigned char *out4Bytes) {
   if (in3Bytes==NULL || out4Bytes==NULL) return;
   out4Bytes[0]=TABLE_BYTE_0_B64[in3Bytes[0]];
   out4Bytes[1]=TABLE_BYTE_3_B64[((in3Bytes[0]&0x03)<<4)+
                                 ((in3Bytes[1]&0xf0)>>4)];
   out4Bytes[2]=TABLE_BYTE_3_B64[((in3Bytes[1]&0x0f)<<2)+
                                 ((in3Bytes[2]&0xc0)>>6)];
   out4Bytes[3]=TABLE_BYTE_3_B64[in3Bytes[2]];
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                   encodeToBase64
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Base64Encoder::encode(int n, unsigned char *inBytes, unsigned char *outBytes) {
  if (inBytes==NULL || outBytes==NULL || n<=0) return 0;
  int m=n-(n%3);
  int i, j;
  for ( i=j=0; i<m; i+=3, j+=4)  encode(inBytes+i,outBytes+j);
  if (m!=n) {
   char unsigned lastchars[3] = {0,0,0};
   lastchars[0]=inBytes[i];
   if ((n-m)==2) lastchars[1]=inBytes[i+1];
   encode(lastchars,outBytes+j);
   outBytes[j+3]='=';
   if ((n-m)==1) outBytes[j+2]='=';
   j+=4;
  }
  return j;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                   runencodeToBase64
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Base64Encoder::runEncode(int n, unsigned char *inBytes, unsigned char *outBytes, bool flushBuffer) {
  static int cachedChars=0;
  static unsigned char charCache[3];
  int l=0;

  // there are cached leftovers
  if (cachedChars>0) {
    // fill more
    if (n>0) { charCache[cachedChars++]=*inBytes; inBytes++; n--; }
    if (cachedChars<3 && n>0) { charCache[cachedChars++]=*inBytes; inBytes++; n--; }
    // if filled
    if (cachedChars==3 || flushBuffer) {
     l=encode(cachedChars,charCache,outBytes);
     outBytes+=l;
     cachedChars=0;
    }
  }
  if (n==0) return l;

  int m=n-n%3;
  if (flushBuffer || n==m) {
     l+=encode(n,inBytes,outBytes);
     return l;
  }
  // cache left overs
  charCache[cachedChars++]=inBytes[m];
  if (m+1<n) charCache[cachedChars++]=inBytes[m+1];
  l+=encode(m,inBytes,outBytes);
   return l;
}

void Base64Encoder::runEncodeAndWrite(FILE *fptr, unsigned char *inBytes, int n, bool flushBuffer) {
   while (n>128) {
      int nleftEncoded=runEncode(128,inBytes,encodedBuffer, false);
      if (nleftEncoded>0) {
        encodedBuffer[nleftEncoded]=0;
        fputs((const char *)encodedBuffer,fptr);
      }
      n-=128;
   }
   int nEncoded=runEncode(n,inBytes,encodedBuffer, flushBuffer);
   if (nEncoded>0) {
     encodedBuffer[nEncoded]=0;
     fputs((const char *)encodedBuffer,fptr);
   }
}

void Base64Encoder::runEncodeAndWriteInt(FILE *fptr, int i, bool flushBuffer) {
   int nEncoded=runEncode(sizeof(int),(unsigned char*)&i,encodedBuffer, flushBuffer);
   if (nEncoded>0) {
     encodedBuffer[nEncoded]=0;
     fputs((const char *)encodedBuffer,fptr);
   }
}

void Base64Encoder::runEncodeAndWriteFlush(FILE *fptr) {
  int nEncoded=runEncode(0,NULL,encodedBuffer, true);
   if (nEncoded>0) { 
     encodedBuffer[nEncoded]=0; 
     fputs((const char *)encodedBuffer,fptr); 
   }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                                   flip N bytes
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define flip2Bytes(str) {char c=str[1];str[1]=str[0];str[0]=c;}
#define flip4Bytes(str) {char c=str[3];str[3]=str[0];str[0]=c; c=str[1];str[1]=str[2];str[2]=c;}
#define flip8Bytes(str) {char c=str[7];str[7]=str[0];str[0]=c; \
			      c=str[6];str[6]=str[1];str[1]=c; \
			      c=str[5];str[5]=str[2];str[2]=c; \
			      c=str[4];str[4]=str[3];str[3]=c; }

#define flipNBytes(n,str) {for (int _flip_i=n/2;_flip_i<n;_flip_i++) \
			     {char c=str[_flip_i];str[_flip_i]=str[n-1-_flip_i];str[n-1-_flip_i]=c;}};

void Base64Encoder::flipBytes(int size, int n, unsigned char *data) {
  if (size==2)      for (int i=0; i<n; i++, data+=2)    {flip2Bytes(data);}
  else if (size==4) for (int i=0; i<n; i++, data+=4)    {flip4Bytes(data);}
  else if (size==8) for (int i=0; i<n; i++, data+=8)    {flip8Bytes(data);}
  else              for (int i=0; i<n; i++, data+=size) {flipNBytes(size,data);}
}
