#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/Base64Encoder.h"

#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define null NULL

bool IS_JSON=true;
int  headersize=0;
int  skipafterheader=0;

int     n=91200;
float * buffer;
int     buffersize=0;
int     nbytes=2;

unsigned short *sptr;
unsigned char  *ptr;
unsigned char  *str;

int main(int nargs, char **args) {
   for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
       return 0;
     } else if (strequals(args[i],"-byte")) {
       nbytes=1;
     } else if (strequals(args[i],"-short")) {
       nbytes=2;
     } else if (strequals(args[i],"-n")) {
       sscanf(args[++i],"%i",&n);
     } else if (strequals(args[i],"-fromjson")) {
       IS_JSON=true;
     } else if (strequals(args[i],"-headersize")) {
       sscanf(args[++i],"%i",&headersize);
     } else if (strequals(args[i],"-skipafterheader")) {
       sscanf(args[++i],"%i",&skipafterheader);
     }
   }

   if (headersize>0) {
      char strbuffer[512];
      for (int i=0; i<headersize; i++) {
         fgets(strbuffer,512,stdin);
         fputs(strbuffer,stdout);
      }
      for (int i=0; i<skipafterheader; i++)
         fgets(strbuffer,512,stdin);
   }

   buffersize=sizeof(float)*n;
   buffer=(float *)malloc(sizeof(float)*n);
   if (buffer==NULL) return 1;

   int maxstr=buffersize*3/2+12;
   str=(unsigned char*)malloc(maxstr);
   if (str==NULL) return 1;

   ptr= (unsigned char  *)buffer;
   sptr=(unsigned short *)buffer;

   float f;
   unsigned short s;
   int v;
   for (int i=0; i<n; i++) {

     if (IS_JSON && i<n-1)
       scanf("%f,",&f);
     else
       scanf("%f",&f);

     v=(f-2.0f)/2.0f; if (f<0.0f) f=0.0f; if (f>1.0f)  f=1.0f;
     if (nbytes==1) {
       v=(f*256);     if (v<0)    v=0;    if (v>255)   v=255;
       ptr[i]= (unsigned char)  v;
     } else if (nbytes==2) {
       v=(f*65536);   if (v<0)    v=0;    if (v>65535) v=65535;
       sptr[i]=(unsigned short) v;
     } else {
       fprintf(stderr,"Error. nbytes %i not supported.\n",nbytes);
       return 1;
     }

   }

   Base64Encoder::encode(n*nbytes,ptr,str);

   if (IS_JSON) putchar('"');

   for (int i=0; i<maxstr && str[i]>0; i++)
      putchar(str[i]);

   if (IS_JSON) printf("\"\n}\n");

   return 0;
}
