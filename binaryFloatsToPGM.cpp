#include <stdio.h>
#include <stdlib.h>

int main() {
  int count=1204980;
  float f;
  float minf=-0.047312;
  float maxf=1.013830;
  float df=maxf-minf;

  printf("P5\n%i %i\n",285,4228);

  while (scanf("%f",&f)==1) {
    int i=(f-minf)*256.0/df;
    if (i<0) i=0;
    else if (i>255) i=255;
    char c=(unsigned char)i;
    putchar(c);
    //printf("i: %i | c: %i | c:%c\n",i,c,c);
  }

  return 0;
}
