#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define null NULL

int band_red   = 236;
int band_green = 114;
int band_blue  = 32;


int main(int nargs, char **args) {
  //    min max: -0.182626 .. 2.111865
  float  minf= -0.047312;
  float  maxf=  1.013830;
  int    w=         320;
  int    h=        4228;
  int    nsamples=  285;

  for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
        printf("%s: -h\n",args[0]);
        printf("%s: [-width <width> | -nsamples <nsamples> | -height <height>]\n",args[0]);
        printf("%s: [-red <red_band>] [-green <green_band>] [-blue <blue_band>]\n",args[0]);
        return 0;
     } else if (strequals(args[i],"-red")) {
        sscanf(args[++i],"%i",&band_red);
     } else if (strequals(args[i],"-green")) {
        sscanf(args[++i],"%i",&band_green);
     } else if (strequals(args[i],"-blue")) {
        sscanf(args[++i],"%i",&band_blue);
     } else if (strequals(args[i],"-rgb")) {
        sscanf(args[++i],"%i",&band_red);
        sscanf(args[++i],"%i",&band_green);
        sscanf(args[++i],"%i",&band_blue);
     } else if (strequals(args[i],"-bounds")) {
       sscanf(args[++i],"%f",&minf);
       sscanf(args[++i],"%f",&maxf);
     } else if (strequals(args[i],"-width")) {
       sscanf(args[++i],"%i",&w);
     } else if (strequals(args[i],"-nsamples")) {
       sscanf(args[++i],"%i",&nsamples);
     } else if (strequals(args[i],"-height")) {
       sscanf(args[++i],"%i",&h);
     } else if (strequals(args[i],"-min")) {
       sscanf(args[++i],"%f",&minf);
     } else if (strequals(args[i],"-max")) {
       sscanf(args[++i],"%f",&maxf);
     }
  }

  band_red--;
  band_green--;
  band_blue--;

  long unsigned int slicesize= w*nsamples;
  float *buffer=(float *)malloc(sizeof(float)*slicesize);

  float df=maxf-minf;

  printf("P6\n# width %i | bands %i x height %i\n%i %i %i\n",w,nsamples,h,w,h,255);

  for (int j=0; j<h; j++) {
    fread(buffer,sizeof(float),slicesize,stdin);
    for (int i=0; i<w; i++) {
       int r=(int)((buffer[i+w*band_red]  -minf)*256.0f/df);
       if (r<0) r=0; else if (r>255) r=255;
       int g=(int)((buffer[i+w*band_green]-minf)*256.0f/df);
       if (g<0) g=0; else if (g>255) g=255;
       int b=(int)((buffer[i+w*band_blue] -minf)*256.0f/df);
       if (b<0) b=0; else if (b>255) b=255;
       putchar(r);
       putchar(g);
       putchar(b);
    }
  }

  return 0;
}
