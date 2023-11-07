#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strequals(s0,s1) (!strcmp((s0),(s1)))

int main(int nargs, char **args) {
  int count=1204980;
  float f;
  float minf=-0.047312;
  float maxf=1.013830;
  int w=295;
  int h=4228;

  for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
        return 0;
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
     }
  }

  float df=maxf-minf;

  printf("P5\n%i %i %i\n",285,4228,255);

  while (scanf("%f",&f)==1) {
    int i=(f-minf)*256.0/df;
    if (i<0) i=0;
    else if (i>255) i=255;
    char c=(unsigned char)i;
    putchar(c);
    //printf("i; %i | c; %i | c;%c\n",i,c,c);
  }

  return 0;
}
