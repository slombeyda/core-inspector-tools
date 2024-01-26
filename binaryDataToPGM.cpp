#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define warn(s) fprintf(stderr,"%s\n",s)
#define warns(fs,s) fprintf(stderr,fs,s)

#define null NULL

const int REDUCE_MODE_CLAMP = 0;
const int REDUCE_MODE_IGNORE = 1;
const int REDUCE_MODE_AVERAGE = 2;

const int DATA_MODE_INT   = 0;
const int DATA_MODE_FLOAT = 1;

const int  MAX_MODE_NONE           =0;
const int  MAX_MODE_FACTOR         =1;
const int  MAX_MODE_AUTO_PER_GLOBAL=2;
const int  MAX_MODE_AUTO_PER_PIECE =3;

int maxmode= MAX_MODE_AUTO_PER_PIECE;
int datamode=DATA_MODE_FLOAT;
bool enhancedmode=false;
double enhancedexp =1;

int width =-1;
int height=-1;
int reducefactor=16;
int skipheader=-1;

int reduceMode= REDUCE_MODE_IGNORE;

const int FORMAT_PGM  = 0;
const int FORMAT_JSON = 1;

int outputformat = FORMAT_PGM;

bool quietmode=false;

FILE* fptr= stdin;

void readInBounds(float *min_value, float *max_value,FILE *fptr,int w, int h, int datamode) {
   if (datamode==DATA_MODE_FLOAT) {
     int count=0;
     float minv,maxv,v;
     fread(&v,sizeof(float),1,fptr);
     minv=maxv=v;
     count ++;
     for (; count<w*h; count++) {
        fread(&v,sizeof(float),1,fptr);
        if (v>maxv) maxv=v;
        if (v<minv) minv=v;
     }
     min_value[0]=minv;
     max_value[0]=maxv;
     fseek(fptr,-sizeof(float)*w*h,SEEK_CUR);
   }
}


void readFloats(FILE *fptr, int nacross, int ndown, int width, int height, int reducefactor,
               int kernelsize, float upperbound, float *readbuffer, float *midbuffer, unsigned char *outbuffer) {
  for (int j=0; j<ndown; j++)  {
     fread(readbuffer,sizeof(int),width*reducefactor,fptr);
      for (int i=0; i<nacross; i++) {
          midbuffer[i]=0;
          for (int py=0; py<reducefactor; py++) {
              int index=i*reducefactor+py*width;
              for (int px=0; px<reducefactor; px++,index++) {
                   midbuffer[i]+=readbuffer[index];
              }
          }
          // average across kernel, and normalize
          if (enhancedmode) {
            double v=(double)midbuffer[i]/upperbound/(double)kernelsize;
            v=v*M_PI/2.0;
            v=cos(v);
            v=pow(v,enhancedexp);
            v=1.0-v;
            midbuffer[i]=(float)(v*256.0);
          } else
            midbuffer[i]=midbuffer[i]*256.0f/upperbound/(float)kernelsize;
          if (midbuffer[i]<0.0f)    midbuffer[i]=0.0f;
          if (midbuffer[i]>=255.0f) midbuffer[i]=255.0f;
          outbuffer[i]=(unsigned char)((unsigned int)midbuffer[i]);
      }
      if (outputformat==FORMAT_PGM)
        fwrite(outbuffer,1,nacross,stdout);
      else if (outputformat==FORMAT_JSON) {
        for (int i=0; i<nacross; i++) {
            printf("%i",outbuffer[i]);
            if (i==nacross-1) {
                if (j<ndown-1) putchar(',');
                putchar('\n');
            } else putchar(',');
        }
      }
  }
}

void readOnOff(FILE *fptr, int nacross, int ndown, int width, int height, int reducefactor,
               int kernelsize, int kernelupsize, unsigned int *readbuffer, unsigned int *midbuffer, unsigned char *outbuffer) {
  for (int j=0; j<ndown; j++)  {
     fread(readbuffer,sizeof(int),width*reducefactor,fptr);
      for (int i=0; i<nacross; i++) {
          midbuffer[i]=0;
          for (int py=0; py<reducefactor; py++) {
              int index=i*reducefactor+py*width;
              for (int px=0; px<reducefactor; px++,index++) {
                  if (readbuffer[index]>1)
                     midbuffer[i]++;
              }
          }
          if (kernelsize==256)
            outbuffer[i]=(unsigned char)(midbuffer[i]>255?255:midbuffer[i]);
          else if (kernelsize<256) {
            midbuffer[i]=(unsigned int)(midbuffer[i]*kernelupsize);
            outbuffer[i]=(unsigned char)(midbuffer[i]>255?255:midbuffer[i]);
          } else {
            midbuffer[i]=(unsigned int)( midbuffer[i]*256/kernelsize);
            outbuffer[i]=(unsigned char)(midbuffer[i]>255?255:midbuffer[i]);
          }
      }
      if (outputformat==FORMAT_PGM)
        fwrite(outbuffer,1,nacross,stdout);
      else if (outputformat==FORMAT_JSON) {
        for (int i=0; i<nacross; i++) {
            printf("%i",outbuffer[i]);
            if (i==nacross-1) {
                if (j<ndown-1) putchar(',');
                putchar('\n');
            } else putchar(',');
        }
      }
  }
}


int main(int nargs, char **args) {

   int band=-1;
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
     } else if (strequals(args[narg],"-onoff")) {
       datamode=DATA_MODE_INT;
     } else if (strequals(args[narg],"-float")) {
       datamode=DATA_MODE_FLOAT;
     } else if (strequals(args[narg],"-enhanced")) {
       datamode=DATA_MODE_FLOAT;
       enhancedmode=true;
       sscanf(args[++narg],"%lf",&enhancedexp);
     } else if (strequals(args[narg],"-pgm")) {
       outputformat = FORMAT_PGM;
     } else if (strequals(args[narg],"-json")) {
       outputformat = FORMAT_JSON;
     } else if (strequals(args[narg],"-quiet")) {
       quietmode=true;
     } else if (strequals(args[narg],"-band")) {
       band=0;
       sscanf(args[++narg],"%i",&band);
       if (skipheader<0) skipheader=0;
       skipheader+=band*sizeof(int)*width*height;
     } else if (width<0) {
       sscanf(args[narg],"%i",&width);
     } else if (height<0) {
       sscanf(args[narg],"%i",&height);
     } else if (fptr==stdin) {
       fptr=fopen(args[narg],"rb");
       if (fptr==null) {
          fprintf(stderr,"Error: cound not open %s for reading.",args[narg]);
          return 1;
       }
     } else  {
       if (!quietmode)
         warns("unrecognized parameter [%s]",args[narg]);
       return 0;
     }
   }

   if (width%reducefactor!=0)
     if (!quietmode)
       fprintf(stderr,"Warning: width does not reduce evenly to factor. Ignoring %i columns.\n",width%reducefactor);

   if (height%reducefactor!=0)
     if (!quietmode)
       fprintf(stderr,"Warning: height does not reduce evenly to factor. Ignoring %i rows.\n",height%reducefactor);

   int nacross=width/reducefactor;
   int ndown  =height/reducefactor;

   int kernelsize=reducefactor*reducefactor;

   double kernelupsize=256.0/kernelsize;
   double kerneldownsize=kernelsize/256.0;

   unsigned int * readbuffer= (unsigned int *)malloc(sizeof(int)*width*reducefactor);
   unsigned int * midbuffer = (unsigned int *)malloc(nacross*sizeof(int));
   unsigned char* outbuffer = (unsigned char*)malloc(nacross*sizeof(char));

   if (skipheader>0) fseek(fptr,skipheader,SEEK_CUR);

   float minv,maxv;
   bool calculatebounds=true;
   if (calculatebounds) readInBounds(&minv,&maxv,fptr,width,height,datamode);

   //if (DEBUG) fprintf(stderr,"%i: %f .. %f\n",band,minv,maxv);

   if (outputformat==FORMAT_PGM)
     printf("P5\n%i %i\n%i\n",nacross,ndown,255);
   else if (outputformat==FORMAT_JSON)
     printf("{\n  \"width\":%i,\n  \"height\":%i,\n  \"maxgray\":%i,\n  \"data\":\n[\n",nacross,ndown,255);

   float upperbound=maxv;
   if (maxmode==MAX_MODE_AUTO_PER_PIECE)
      upperbound=maxv;

   if (datamode==DATA_MODE_INT)
     readOnOff(fptr, nacross, ndown, width, height, reducefactor, kernelsize, kernelupsize, readbuffer, midbuffer, outbuffer);
   else
     readFloats(fptr, nacross, ndown, width, height, reducefactor, kernelsize, upperbound, (float *)readbuffer, (float *)midbuffer, outbuffer);

   if (outputformat==FORMAT_JSON)
     printf("]\n}\n");

   return 0;
}
