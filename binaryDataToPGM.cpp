#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int REDUCE_MODE_CLAMP = 0;
const int REDUCE_MODE_IGNORE = 1;
const int REDUCE_MODE_AVERAGE = 2;

int width =-1;
int height=-1;
int reduceside=16;

int main(int nargs, char **args) {

   for (int narg=1; narg<nargs; narg++) {
     if (!strcmp(args[narg],"-h")) {
     } else if (!strcmp(args[narg],"-width")) {
       sscanf(args[++narg],"%i",&width);
     } else if (!strcmp(args[narg],"-height")) {
       sscanf(args[++narg],"%i",&height);
     } else if (!strcmp(args[narg],"-factor")) {
       sscanf(args[++narg],"%i",&reduceside);
     } else if (width<0) {
       sscanf(args[narg],"%i",&width);
     } else if (height<0) {
       sscanf(args[narg],"%i",&height);
     }
   }

   if (width%reduceside!=0)
     fprintf(stderr,"width does not reduce nicely to factor. %i/%i -r-> %i\n",width,reduceside,width%reduceside);

   if (height%reduceside!=0)
     fprintf(stderr,"height does not reduce nicely to factor. %i/%i -r-> %i\n",height,reduceside,height%reduceside);

   return 0;

}
