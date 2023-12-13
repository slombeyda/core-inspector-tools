#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define null NULL

int main(int nargs, char **args) {
  int    count=1204980;
  float  minf=-0.047312;
  float  maxf=1.013830;
  int    w=320;
  int    h=4228;
  int    nsamples=285;

  bool   autobounds=false;
  bool   printboundsonly=false;

  char * filename=null;
  FILE * fptr=stdin;

  for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
        return 0;
     } else if (strequals(args[i],"-autobounds")) {
        autobounds=true;
     } else if (strequals(args[i],"-printboundsonly")) {
        autobounds=true;
        printboundsonly=true;
     } else if (strequals(args[i],"-width")) {
       i++;
       sscanf(args[i],"%i",&w);
     } else if (strequals(args[i],"-height")) {
       i++;
       sscanf(args[i],"%i",&h);
     } else if (strequals(args[i],"-min")) {
       i++;
       sscanf(args[i],"%f",&minf);
     } else if (strequals(args[i],"-max")) {
       i++;
       sscanf(args[i],"%f",&maxf);
     } else if (strequals(args[i],"-bounds")) {
       i++;
       sscanf(args[i],"%f",&minf);
       i++;
       sscanf(args[i],"%f",&maxf);
     } else if (filename==null) {
       filename=args[i];
     }
  }

  if (filename!=null) {
    fptr=fopen(filename,"rb");
    if (fptr==null) {
       fprintf(stderr,"ERROR: could not open %s for reading.\n",filename);
       return 1;
    }
  }

  if (autobounds) {
    if (fptr==stdin) {
       fprintf(stderr,"ERROR: can only do -autobounds if filename given as last argument.\n");
       return 1;
    }
    fread(&minf,sizeof(float),1,fptr);
    maxf=minf;
    float f;
    //while (fscanf(fptr,"%f",&f)==1)
    while (fread(&f,sizeof(float),1,fptr)>0) {
      if (f>maxf) maxf=f; else
      if (f<minf) minf=f;
    }
    if (printboundsonly) {
      printf("%12f .. %12f\n",minf,maxf);
      fclose(fptr);
      return 0;
    } else {
      fprintf(stderr,"bounds: %f .. %f\n",minf,maxf);
      rewind(fptr);
    }
  }

  float df=maxf-minf;

  if (df==0.0f) df=1.0f;

  printf("P5\n%i %i %i\n",nsamples*w,h,255);
  //while (fscanf(fptr,"%f",&f)==1)
  float f;
  while (fread(&f,sizeof(float),1,fptr)>0) {
    int i=(f-minf)*256.0/df;
    if (i<0) i=0;
    else if (i>255) i=255;
    char c=(unsigned char)i;
    putchar(c);
  }

  return 0;
}
