#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const int DATA_BYTE =0;
const int DATA_INT  =1;
const int DATA_FLOAT=2;

int datamode=DATA_BYTE;

int main (int nargs, char**args) {

  if (nargs>1) {
     if (strcmp(args[1],"-byte" )==0) datamode=DATA_BYTE;
     else
     if (strcmp(args[1],"-int"  )==0) datamode=DATA_INT;
     else
     if (strcmp(args[1],"-float")==0) datamode=DATA_FLOAT;
  }

  if (datamode==DATA_INT) {
    int i=0;
    while (fread(&i,4,1,stdin)) {
      printf("%i\n",i);
    }

  } else if (datamode==DATA_FLOAT) {
    float f=0;
    while (fread(&f,4,1,stdin)) {
      if (isnan(f)) {}
      else printf("%f\n",f);
    }

  } else {
    const int n_per_row=16;
    unsigned char c=0;
    int count=0;
    char s[22];

    s[0]=s[1]=s[2]=s[3]=' ';
    s[16+4]='\n';
    s[17+4]=0;

    while (fread(&c,1,1,stdin)) {
      int ic=count%16;
      if (c>=32 && c<=136) s[ic+4]=c;
      else s[ic+4]='.';
      count++;
      printf("%02X%s",c,(count%16)==0?s:" ");
    }
    if ((count%16)!=0) {
      s[4+(count%16)]=0;
      printf("%s\n",s);
    }

  }
  return 0;
}
