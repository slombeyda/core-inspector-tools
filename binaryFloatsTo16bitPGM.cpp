#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define null NULL


void printPGMHeader(FILE *foutptr, int w, int h, float minv, float maxv) {
  fprintf(foutptr,"P5\n# width %i x height %i | data %f..%f\n%i %i %i\n",
                    w,h,
                    minv, maxv,
		    w,h,65535);
}


int main(int nargs, char **args) {
  //    min max: -0.182626 .. 2.111865
  float  minf= 0;
  float  maxf= 52.478104;
  int    w=    320;
  int    h=    2947;
  int    count=943040;

  FILE * fptr=stdin;
  FILE * foutptr=stdout;
  char * filename=null;

  float          *buffer    =null;
  unsigned short *outbuffer =null;
  unsigned char  *coutbuffer=null;
  unsigned char   c;

  for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
        printf("%s: -h\n",args[0]);
        printf("       [-min <min float> | -max <max float> | bounds <min float> <max float>]\n");
        return 0;
     } else if (strequals(args[i],"-width")) {
       sscanf(args[++i],"%i",&w);
       count=w*h;
     } else if (strequals(args[i],"-height")) {
       sscanf(args[++i],"%i",&h);
       count=w*h;
     } else if (strequals(args[i],"-min")) {
       sscanf(args[++i],"%f",&minf);
     } else if (strequals(args[i],"-max")) {
       sscanf(args[++i],"%f",&maxf);
     } else if (strequals(args[i],"-bounds")) {
       sscanf(args[++i],"%f",&minf);
       sscanf(args[++i],"%f",&maxf);
     } else if (filename==null) {
       filename=args[i];
     }
  }

  // OPEN [BASE] INPUT FILE
  if (filename!=null) {
    fptr=fopen(filename,"rb");
    if (fptr==null) {
       fprintf(stderr,"ERROR: could not open %s for reading.\n",filename);
       return 1;
    }
  }

     buffer=(float          *)malloc(sizeof(float         )*w*2);
  outbuffer=(unsigned short *)malloc(sizeof(unsigned short)*w*2);
  coutbuffer=(unsigned char *)outbuffer;

  printPGMHeader(foutptr,w,h,minf,maxf);

  bool FLIP_SHORT_BYTES = true;

  while (fread(buffer,sizeof(float),w,fptr)>0) {
    for (int index=0; index<w; index++) {

        if (isnan(buffer[index]))    buffer[index]=minf;
        else if (buffer[index]<minf) buffer[index]=minf;
        else if (buffer[index]>maxf) buffer[index]=maxf;

        int v=(int)((buffer[index]-minf)*65536.0/(maxf-minf));

             if (v<0)      v=0;
        else if (v>=65536) v=65535;

        outbuffer[index]=(unsigned short)v;

    }

    if (FLIP_SHORT_BYTES) {
      for (int i=0;i<w*2; i+=2) {
         c=coutbuffer[i];
         coutbuffer[i]=coutbuffer[i+1];
         coutbuffer[i+1]=c;
      }
    }

    fwrite(outbuffer,sizeof(short),w,foutptr);
  }

  if (fptr!=stdin)     fclose(foutptr);
  if (foutptr!=stdout) fclose(foutptr);

  return 0;
}
