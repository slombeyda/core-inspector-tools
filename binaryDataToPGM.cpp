#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define warn(s) fprintf(stderr,"%s\n",s)
#define warns(fs,s) fprintf(stderr,fs,s)

const int REDUCE_MODE_CLAMP = 0;
const int REDUCE_MODE_IGNORE = 1;
const int REDUCE_MODE_AVERAGE = 2;

int width =-1;
int height=-1;
int reducefactor=16;

int skipheader=-1;

int reduceMode= REDUCE_MODE_IGNORE;

int main(int nargs, char **args) {

   for (int narg=1; narg<nargs; narg++) {

     if (!strcmp(args[narg],"-h")) {
       warns("usage: %s -h\n",args[0]);
       warns("usage: %s [-width <width>] []-height <height>] [-jumptohalf] [<width> <height>]\n",args[0]);
       return 0;

     } else if (strequals(args[narg],"-width")) {
       sscanf(args[++narg],"%i",&width);

     } else if (strequals(args[narg],"-height")) {
       sscanf(args[++narg],"%i",&height);
     } else if (strequals(args[narg],"-factor")) {
       sscanf(args[++narg],"%i",&reducefactor);
     } else if (strequals(args[narg],"-jumptohalf")) {
       if (skipheader<0) skipheader=0;
       skipheader+=width*(height/2)*sizeof(int);
     } else if (strequals(args[narg],"-band")) {
       int band=0;
       sscanf(args[++narg],"%i",&band);
       if (skipheader<0) skipheader=0;
       skipheader+=band*sizeof(int)*width*height;
     } else if (width<0) {
       sscanf(args[narg],"%i",&width);
     } else if (height<0) {
       sscanf(args[narg],"%i",&height);
     } else  {
       warns("unrecognized parameter [%s]",args[narg]);
       return 0;
     }
   }

   if (width%reducefactor!=0)
     fprintf(stderr,"Warning: width does not reduce evenly to factor. Ignoring %i columns.\n",width%reducefactor);

   if (height%reducefactor!=0)
     fprintf(stderr,"Warning: height does not reduce evenly to factor. Ignoring %i rows.\n",height%reducefactor);

   int nacross=width/reducefactor;
   int ndown  =height/reducefactor;

   unsigned int * readbuffer= (unsigned int *)malloc(sizeof(int)*width*reducefactor);
   unsigned char* outbuffer = (unsigned char*)malloc(nacross*sizeof(char));

   if (skipheader>0) fseek(stdin,skipheader,SEEK_CUR);

   printf("P5\n%i %i\n%i\n",nacross,ndown,255);

   for (int j=0; j<ndown; j++)  {
      fread(readbuffer,sizeof(int),width*reducefactor,stdin);
       for (int i=0; i<nacross; i++) {
           outbuffer[i]=0;
           for (int py=0; py<reducefactor; py++) {
               int index=i*reducefactor+py*width;
               for (int px=0; px<reducefactor; px++,index++) {
                   if (readbuffer[index]>1)
                      outbuffer[i]++;
               }
           }
       }
       fwrite(outbuffer,1,nacross,stdout);
   }

   return 0;
}
