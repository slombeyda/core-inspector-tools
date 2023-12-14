#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define null NULL

char *strasspaces(char *s) {
  char *spaces=null;
  int n=strlen(s);
  spaces=(char *)malloc(n+1);
  for (int i=0; i<n; i++) spaces[i]=' ';
  spaces[n]=0;
  return spaces;
}

void printPGMHeader(FILE *foutptr, int w, int h, int nsamples, int nthslicepiece, int nslicesperpiece) {
  int sliceo= nthslicepiece   *nslicesperpiece;
  int slicef=(nthslicepiece+1)*nslicesperpiece-1;
  if (slicef>=h) slicef=h-1;
  int nslices=slicef-sliceo+1;
  fprintf(foutptr,"P5\n# width %i | bands %i x slices %i - %i\n%i %i %i\n",
                    w,nsamples,
                    sliceo,slicef,
                    w,nsamples*nslices,255);
}

void makeOutFilename(char *outfilename,char *fileoutprefix,int h, int nthslicepiece, int nslicesperpiece) {
    int sliceo= nthslicepiece   *nslicesperpiece;
    int slicef=(nthslicepiece+1)*nslicesperpiece-1;
    if (slicef>=h) slicef=h-1;
    snprintf(outfilename,512,"%s.%04i-%04i.pgm",
                             fileoutprefix,
                             sliceo,slicef);
}

const int MODE_PGM         = 0;
const int MODE_TEXT_SHORTS = 1;
const int MODE_TEXT_FLOATS = 2;

int main(int nargs, char **args) {
  //    min max: -0.182626 .. 2.111865
  int    count=1204980;
  float  minf=-0.047312;
  float  maxf=1.013830;
  int    w=320;
  int    h=4228;
  int    nsamples=285;

  bool   autobounds=false;
  bool   printboundsonly=false;
  bool   outputinpieces=false;
  int    nslicesperpiece=16;

  char * fileoutprefix=null;
  char * filename=null;
  FILE * fptr=stdin;
  FILE * foutptr=stdout;
  char   outfilename[512];


  int outputmode=MODE_PGM;

  for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
        printf("%s: -h\n",args[0]);
        printf("%s: [-width <width> | -nsamples <nsamples> | -height <height>]\n",args[0]);
        printf("%s: [-pgm | -textshorts | -textfloats]\n",args[0]);
        printf("%s  [-autobounds | -min <min float> | -max <max float> | bounds <min float> <max float>]\n",strasspaces(args[0]));
        printf("%s  [-outputinpieces -slicesperpiece <n slices> -fileoutprefix <filename base>]\n",strasspaces(args[0]));
        return 0;
     } else if (strequals(args[i],"-pgm")) {
        outputmode=MODE_PGM;
     } else if (strequals(args[i],"-textshorts")) {
        outputmode=MODE_TEXT_SHORTS;
     } else if (strequals(args[i],"-textfloats")) {
        outputmode=MODE_TEXT_FLOATS;
     } else if (strequals(args[i],"-outputinpieces")) {
        outputinpieces=true;
     } else if (strequals(args[i],"-slicesperpiece")) {
        i++;
        outputinpieces=true;
        sscanf(args[i],"%i",&nslicesperpiece);
     } else if (strequals(args[i],"-fileoutprefix")) {
        fileoutprefix=args[++i];
     } else if (strequals(args[i],"-autobounds")) {
        autobounds=true;
     } else if (strequals(args[i],"-printboundsonly")) {
        autobounds=true;
        printboundsonly=true;
     } else if (strequals(args[i],"-width")) {
       i++;
       sscanf(args[i],"%i",&w);
     } else if (strequals(args[i],"-nsamples")) {
       i++;
       sscanf(args[i],"%i",&nsamples);
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

  int nthpiece=0;
  int nthslicepiece=0;

  if (outputinpieces) {
    if (fileoutprefix==null) {
      fprintf(stderr,"Error: must define -fileoutprefix <filename out prefix> to be able to export separete files.\n");
      return 1;
    }
    makeOutFilename(outfilename,fileoutprefix,h,nthslicepiece,nslicesperpiece);
    foutptr=fopen(outfilename,"wb");
    printPGMHeader(foutptr,w,h,nsamples,nthslicepiece,nslicesperpiece);
  } else
    fprintf(foutptr,"P5\n# width %i | bands %i x height %i\n%i %i %i\n",w,nsamples,h,w,nsamples*h,255);

  unsigned long int nthfloat=0;
  float f;
  while (fread(&f,sizeof(float),1,fptr)>0) {
    int index_in_slice_piece=nthfloat%(w*nsamples*nslicesperpiece);
    if (outputinpieces && index_in_slice_piece==0) {
      if (nthfloat>0) {
         fclose(foutptr);
         nthslicepiece++;
         makeOutFilename(outfilename,fileoutprefix,h,nthslicepiece,nslicesperpiece);
         foutptr=fopen(outfilename,"wb");
         printPGMHeader(foutptr,w,h,nsamples,nthslicepiece,nslicesperpiece);
      }
    }
    int i=(f-minf)*256.0/df;
    if (i<0) i=0;
    else if (i>255) i=255;
    char c=(unsigned char)i;
    if (outputmode==MODE_PGM)
      fputc(c,foutptr);
    else if (outputmode==MODE_TEXT_FLOATS)
      fprintf(foutptr,"%f,\n",f);
    else if (outputmode==MODE_TEXT_SHORTS)
      fprintf(foutptr,"%i,\n",i);
    nthfloat++;
  }

  if (foutptr!=stdout) fclose(foutptr);

  return 0;
}
