#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define strequals(s0,s1) (!strcmp((s0),(s1)))
#define null NULL


const int MODE_TO_BYTES  = 0;
const int MODE_TO_SHORTS = 1;
const int MODE_FLOATS    = 2;

const int FILE_FORMAT_PGM    = 0;
const int FILE_FORMAT_JSON   = 1;
const int FILE_FORMAT_CSV    = 2;
const int FILE_FORMAT_ASCII  = 3;
const int FILE_FORMAT_BIN    = 4;
const int FILE_FORMAT_PPM    = 5;

const char* FILE_FORMAT_EXTENSIONS[] = { "pgm","json","csv","txt","raw","ppm"};


int band_red   = 236;
int band_green = 114;
int band_blue  = 32;


char *strasspaces(char *s) {
  char *spaces=null;
  int n=strlen(s);
  spaces=(char *)malloc(n+1);
  for (int i=0; i<n; i++) spaces[i]=' ';
  spaces[n]=0;
  return spaces;
}


void printPGMHeader(FILE *foutptr, int w, int nsamples, int h, int nthslicepiece, int nslicesperpiece) {
  int sliceo= nthslicepiece   *nslicesperpiece;
  int slicef=(nthslicepiece+1)*nslicesperpiece-1;
  if (slicef>=h) slicef=h-1;
  int nslices=slicef-sliceo+1;
  fprintf(foutptr,"P5\n# width %i | bands %i x slices %i - %i\n%i %i %i\n",
                    w,nsamples,
                    sliceo,slicef,
                    w,nsamples*nslices,255);
}


void printPPMHeader(FILE *foutptr, int w, int nsamples, int h, int nthslicepiece, int nslicesperpiece) {
  int sliceo= nthslicepiece   *nslicesperpiece;
  int slicef=(nthslicepiece+1)*nslicesperpiece-1;
  if (slicef>=h) slicef=h-1;
  int nslices=slicef-sliceo+1;
  fprintf(foutptr,"P6\n# width %i | bands %i x slices %i - %i\n%i %i %i\n",
                    w,nsamples,
                    sliceo,slicef,
                    w,nsamples*nslices,255);
}



void printJSONHeader(FILE *foutptr, int w, int nsamples, int h,
                                    int w_reducefactor, int nsamples_reducefactor, int h_reducefactor,
                                    int w_f, int nsamples_f, int h_f,
                                    int start_slice, int end_slice, float minf, float maxf) {
      fprintf(foutptr,"{\n  \"width\":%i,\n"
                         "  \"nsamples\":%i,\n"
                         "  \"height\":%i,\n"
                         "  \"width_reducefactor\":%i,\n"
                         "  \"nsamples_reducefactor\":%i,\n"
                         "  \"height_reducefactor\":%i,\n"
                         "  \"width_reduced\":%i,\n"
                         "  \"nsamples_reduced\":%i,\n"
                         "  \"height_reduced\":%i,\n"
                         "  \"start_slice\":%i,\n"
                         "  \"end_slice\":%i,\n"
                         "  \"min_value\":%f,\n"
                         "  \"max_value\":%f,\n"
                         "  \"data\":\n[\n",
                          w,nsamples,h,
                          w_reducefactor,nsamples_reducefactor,h_reducefactor,
                          w_f,nsamples_f,h_f,
                          start_slice,end_slice,
                          minf,maxf);
}

void makeOutFilename(char *outfilename,char *fileoutprefix,int h, int nthslicepiece, int nslicesperpiece, int fileformat) {
    int sliceo= nthslicepiece   *nslicesperpiece;
    int slicef=(nthslicepiece+1)*nslicesperpiece-1;
    if (slicef>=h) slicef=h-1;
    snprintf(outfilename,512,"%s.%04i-%04i.%s",
                             fileoutprefix,
                             sliceo,slicef,FILE_FORMAT_EXTENSIONS[fileformat]);
}

void readInMinAndMax(FILE *fptr,float *minf_ptr,float *maxf_ptr, bool printboundsonly) {
  float minf=0;
  float maxf;
    if (fptr==stdin) {
       fprintf(stderr,"ERROR: can only do -autobounds if filename given as last argument.\n");
        exit(1);
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
      exit(0);
    } else {
      fprintf(stderr,"bounds: %f .. %f\n",minf,maxf);
      rewind(fptr);
    }
  minf_ptr[0]=minf;
  maxf_ptr[0]=maxf;
}




int main(int nargs, char **args) {
  //    min max: -0.182626 .. 2.111865
  int    count=1204980;
  float  minf=-0.047312;
  float  maxf=1.013830;
  int    w=320;
  int    h=4228;
  int    nsamples=285;

  bool  reduce=false;
  int   h_reducefactor=1;
  int   w_reducefactor=1;
  int   nsamples_reducefactor=1;
  int   total_reducefactor=1;
  float total_reducefactor_FLOAT=1.0f;

  int    w_f=w;
  int    h_f=h;
  int    nsamples_f=nsamples;

  bool   autobounds=false;
  bool   printboundsonly=false;
  bool   outputinpieces=false;
  int    nslicesperpiece=16;

  char * fileoutprefix=null;
  char * filename=null;
  FILE * fptr=stdin;
  FILE * foutptr=stdout;
  char   outfilename[512];

  int outputmode=MODE_TO_BYTES;
  int fileformat=FILE_FORMAT_PGM;

  for (int i=1; i<nargs; i++) {
     if (strequals(args[i],"-h")) {
        printf("%s: -h\n",args[0]);
        printf("%s: [-width <width> | -nsamples <nsamples> | -height <height>]\n",args[0]);
        printf("%s: [-pgm | -textshorts | -textfloats]\n",args[0]);
        printf("%s  [-autobounds | -min <min float> | -max <max float> | bounds <min float> <max float>]\n",strasspaces(args[0]));
        printf("%s  [-outputinpieces -slicesperpiece <n slices> -fileoutprefix <filename base>]\n",strasspaces(args[0]));
        return 0;
     } else if (strequals(args[i],"-ppm")) {
        outputmode=MODE_TO_BYTES;
        fileformat=FILE_FORMAT_PPM;
     } else if (strequals(args[i],"-pgm")) {
        outputmode=MODE_TO_BYTES;
        fileformat=FILE_FORMAT_PGM;
     } else if (strequals(args[i],"-jsonfloats")) {
        outputmode=MODE_FLOATS;
        fileformat=FILE_FORMAT_JSON;
     } else if (strequals(args[i],"-textbytes")) {
        outputmode=MODE_TO_BYTES;
        fileformat=FILE_FORMAT_ASCII;
     } else if (strequals(args[i],"-textshorts")) {
        outputmode=MODE_TO_SHORTS;
        fileformat=FILE_FORMAT_ASCII;
     } else if (strequals(args[i],"-textfloats")) {
        outputmode=MODE_FLOATS;
        fileformat=FILE_FORMAT_ASCII;
     } else if (strequals(args[i],"-outputinpieces")) {
        outputinpieces=true;
     } else if (strequals(args[i],"-slicesperpiece")) {
        i++;
        outputinpieces=true;
        sscanf(args[i],"%i",&nslicesperpiece);
     } else if (strequals(args[i],"-reduce")) {
        reduce=true;
        sscanf(args[++i],"%i",&w_reducefactor);
        sscanf(args[++i],"%i",&nsamples_reducefactor);
        sscanf(args[++i],"%i",&h_reducefactor);
     } else if (strequals(args[i],"-reducedefault")) {
        reduce=true;
        w_reducefactor=16;
        nsamples_reducefactor=3;
        h_reducefactor=16;
     } else if (strequals(args[i],"-fileoutprefix")) {
        fileoutprefix=args[++i];
     } else if (strequals(args[i],"-autobounds")) {
        autobounds=true;
     } else if (strequals(args[i],"-printboundsonly")) {
        autobounds=true;
        printboundsonly=true;
     } else if (strequals(args[i],"-width")) {
       sscanf(args[++i],"%i",&w);
       w_f=w;
     } else if (strequals(args[i],"-nsamples")) {
       sscanf(args[++i],"%i",&nsamples);
       nsamples_f=nsamples;
     } else if (strequals(args[i],"-height")) {
       sscanf(args[++i],"%i",&h);
       h_f=h;
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

  long unsigned int slicesize=      w*nsamples;
  long unsigned int slicesizebytes= w*nsamples*sizeof(float);


  total_reducefactor=h_reducefactor*w_reducefactor*nsamples_reducefactor;
  total_reducefactor_FLOAT=(float)total_reducefactor;
  w_f=w/w_reducefactor;
  h_f=h/h_reducefactor;
  nsamples_f=nsamples/nsamples_reducefactor;

  int slicesize_f=w_f*nsamples_f;

  // OPEN [BASE] INPUT FILE
  if (filename!=null) {
    fptr=fopen(filename,"rb");
    if (fptr==null) {
       fprintf(stderr,"ERROR: could not open %s for reading.\n",filename);
       return 1;
    }
  }

  if (autobounds) readInMinAndMax(fptr,&minf,&maxf,printboundsonly);

  float df=maxf-minf;
  if (df==0.0f) df=1.0f;

  int nthpiece=0;
  int nthslicepiece=0;
  int dh=nslicesperpiece;
  if (dh>h) dh=h;
  int dh_f= dh/h_reducefactor;

  //if (DEBUG)
  //fprintf(stderr,"With slice size of %li will read %i slices of %li bytes, total %li\n",
  //		  slicesize,nslicesperpiece,slicesizebytes,slicesizebytes*nslicesperpiece);

  float *buffer= (float *)malloc(slicesizebytes*nslicesperpiece);

  if (outputinpieces) {
    if (fileoutprefix==null) {
      fprintf(stderr,"Error: must define -fileoutprefix <filename out prefix> to be able to export separete files.\n");
      return 1;
    }
  } else {
    if (fileformat==FILE_FORMAT_PGM)
      fprintf(foutptr,"P5\n# width %i | bands %i x height %i\n%i %i %i\n",w_f,nsamples_f,h_f,w_f,nsamples_f*h_f,255);
    else if (fileformat==FILE_FORMAT_PPM)
      fprintf(foutptr,"P6\n# width %i | bands %i x height %i\n%i %i %i\n",w_f,nsamples_f,h_f,w_f,nsamples_f*h_f,255);
    else if (fileformat==FILE_FORMAT_JSON)
      fprintf(foutptr,"{\n  \"width\":%i,\n"
                         "  \"nsamples\":%i,\n"
                         "  \"height\":%i,\n"
                         "  \"width_reducefactor\":%i,\n"
                         "  \"nsamples_reducefactor\":%i,\n"
                         "  \"height_reducefactor\":%i,\n"
                         "  \"width_reduced\":%i,\n"
                         "  \"nsamples_reduced\":%i,\n"
                         "  \"height_reduced\":%i,\n"
                         "  \"start_slice\":%i,\n"
                         "  \"end_slice\":%i,\n"
                         "  \"min_value\":%f,\n"
                         "  \"max_value\":%f,\n"
                         "  \"data\":\n[\n",
                          w,nsamples,h,
                          w_reducefactor,nsamples_reducefactor,h_reducefactor,
                          w_f,nsamples_f,h_f,
                          0,h-1,
                          minf,maxf);
  }


  unsigned long int nthslice=0;
  unsigned long nread=0;
  int nNANs=0;
  while (nthslice<h && (nread=fread(buffer,sizeof(float),slicesize*dh,fptr))>0) {
    if (nread!=slicesize*dh)
      fprintf(stderr,"ERROR: underead %li out of %li (%li bytes).\n",nread,slicesize*dh,slicesize*dh*sizeof(float));
    for (int i=0; i<nread; i++)
      if (isnan(buffer[i])) {
         nNANs++;
         buffer[i]=0.0f;
       }

    int index_in_slice_piece=nthslice%nslicesperpiece;

    // if first slice in piece, open foutptr
    if (outputinpieces && index_in_slice_piece==0) {
      if (nthslice>0) {
         if (fileformat==FILE_FORMAT_JSON) {
           fseek(foutptr,-2,SEEK_CUR);
           fprintf(foutptr,"\n]\n}\n");
         }
         fclose(foutptr);
         nthslicepiece++;
       }
       makeOutFilename(outfilename,fileoutprefix,h,nthslicepiece,nslicesperpiece,fileformat);
       foutptr=fopen(outfilename,"wb");
       if (fileformat==FILE_FORMAT_PGM)
         printPGMHeader(foutptr,w_f,nsamples_f,h_f,nthslicepiece,dh_f);
       else if (fileformat==FILE_FORMAT_PPM)
         printPPMHeader(foutptr,w_f,nsamples_f,h_f,nthslicepiece,dh_f);
       else if (fileformat==FILE_FORMAT_JSON)
         printJSONHeader(foutptr, w,nsamples,dh,
                                  w_reducefactor,nsamples_reducefactor,h_reducefactor,
                                  w_f,nsamples_f,dh_f,
                                  nthslice,nthslice+dh-1,
                                  minf,maxf);

    }

    for (int index=0; index<slicesize_f*dh_f; index++) {
      float f=0;
      //float color_f[3]={0,0,0};
      if (total_reducefactor>0) {
        int sindex=   (index%w_f)                 * w_reducefactor                    +
                     ((index/w_f)%nsamples_f     )* nsamples_reducefactor *w          +
                      (index/w_f /nsamples_f     )* h_reducefactor        *w*nsamples;
        for (int kernel_h=0; kernel_h<h_reducefactor; kernel_h++) {
          for (int kernel_nsamples=0; kernel_nsamples<nsamples_reducefactor; kernel_nsamples++) {
            for (int kernel_w=0; kernel_w<w_reducefactor; kernel_w++) {
              int i=sindex+kernel_w+w*(kernel_nsamples+(kernel_h*nsamples));
              //if (isnan(buffer[i])) {
              //  fprintf(stderr,"NAN @ %6i/%6i : %4i %4i %4i\n",
              //               index, slicesize_f*dh_f,
              //              (index%w_f),
              //             ((index/w_f)%nsamples_f     ),
              //              (index/w_f)/nsamples_f
              //          );
              //}
              f+=buffer[i];
              //color_f[0]+=buffer[i+band_red];
              //color_f[1]+=buffer[i+band_green];
              //color_f[2]+=buffer[i+band_blue;
            }
          }
        }
        f=f/total_reducefactor_FLOAT;
      } else f=buffer[index];

      int i=(f-minf)*256.0/df;
      if (i<0) i=0;
      else if (i>255) i=255;
      char c=(unsigned char)i;
      if (fileformat==FILE_FORMAT_PGM)
        fputc(c,foutptr);
      else if (fileformat==FILE_FORMAT_PPM)
        fputc(c,foutptr);
      else if (outputmode==MODE_FLOATS && fileformat==FILE_FORMAT_JSON)
        fprintf(foutptr,"%f,\n",f);
      else if (outputmode==MODE_TO_BYTES && fileformat==FILE_FORMAT_JSON)
        fprintf(foutptr,"%i,\n",i);
      else if (fileformat==FILE_FORMAT_BIN && outputmode==MODE_TO_BYTES)
        fputc(c,foutptr);
      else
        fprintf(stderr,"Warning: incomplete list of data writers. Use float/byte:JSON or grayscale:PGM.\n");
    }
    nthslice+=dh;
    if (nthslice+dh>h) dh=h-nthslice;
    dh_f=dh/h_reducefactor;
  }

  if (foutptr!=stdout) {
    if (fileformat==FILE_FORMAT_JSON) {
      fseek(foutptr,-2,SEEK_CUR);
      fprintf(foutptr,"\n]\n}\n");
    }
    fclose(foutptr);
  }

  return 0;
}
