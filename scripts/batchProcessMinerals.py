#!/usr/bin/env python3
import os
import re
import csv
import sys
import glob
import subprocess
from pathlib import Path

#                                                   CONFIGURATION
# ---------------------------------------------------------------
VERBOSE = False
DRYRUN = False
LOG = True
DEBUG = False

# JUST MIN MAX [CSV default]
WRITEMINMAX = False

# JUST PRESENCE BINARY ON/OFF
WRITEPRESENCEPGM  = False
WRITEPRESENCEJSON = False

# NORMALIZED FLOAT TO BYTE
WRITEJSON         = False
WRITEPGM          = False


#                                                        DEFAULTS
# ---------------------------------------------------------------
RUN_ID = 'BA'

BOREHOLES = ['BA1', 'BA3', 'BA4']
BANDS = []
REDUCEFACTORS = [1]

MINERAL_SET_TARGET = 'VNIRcalib_resampled_map_export'
MINSET_ID = 'SW'

MINERAL_SET_TARGET = 'SWIRcalib_map_export'
MINSET_ID = 'SW'

REDUCE_FACTORS = {
    'RAW': 1,
    'HIGH': 2,
    'LARGE': 4,
    'BASE': 16,
    'SMALL': 64,
    'TMB': 96,
    'MINI': 106,
    'NANO': 160,
}

EHANCEFACTORS = []

reductionTargets = ['RAW']
reductionTargetFactors = [1]
#                                                          UITILS
# ---------------------------------------------------------------


def runCommand(cmd, args, infile, outfile, append=False):
    if DRYRUN:
        strargs=' '.join([' '.join(subargs) for subargs in args])
        print(cmd, strargs, '<', infile, '>', outfile)
    else:
        inptr=open(infile,'r')
        outptr=open(outfile,'a' if append else 'w')
        subprocess.run([os.path.join(BIN_DIR, cmd)] +
                       [item for sublist in args for item in sublist],
                       stdin= inptr,
                       stdout=outptr)

def makeDir(path):
    if DRYRUN:
        print(f'> mkdir -p {path}')
    else:
        os.makedirs(path, exist_ok=True)


def grepKeyValueFromFile(filepath, key):
    with open(filepath) as f:
        for line in f:
            if line.strip().startswith(key):
                try:
                    return int(line.split('=')[1])
                except Exception:
                    return 0
    return 0


#                                                          PATHS
# ---------------------------------------------------------------
HOME = str(Path.home())
DATA_DIR = os.path.join(HOME, f'data-{RUN_ID}')
DEST_DIR = os.path.join(HOME, f'data-www-{RUN_ID}')

REF_DIR = os.path.join(HOME, 'data-summaries')
MIN_REF_FILE = os.path.join(REF_DIR, f'{RUN_ID}_MINERALS-MINMAX.csv')

BIN_DIR = os.path.join(HOME, 'src/core-inspector-tools')


class ImageHandler:

    def __init__(self, borehole, img_path):
        self.borehole = borehole
        self.imgpath = img_path
        self.imgbase = re.sub(r'\.img$', '', img_path)
        self.imghdr = f'{self.imgbase}.hdr'

        self.imgdir = self.imgbase.split('/')[0]
        self.filename = os.path.basename(self.imgbase)

        parts = re.split(r'[_-]', self.imgdir)
        self.boreholedir = parts[0]
        self.sectionZdir = parts[1]
        self.sectiondir = re.sub(r'[ZM]', '', self.sectionZdir)
        self.sectiondirX = self.sectionZdir[-1]
        self.piecedir = parts[2] if len(parts) > 2 else '000'

        self.sectionindex = int(re.sub(r'\D', '', self.sectiondir) or 0)
        self.pieceindex = int(re.sub(r'\D', '', self.piecedir) or 0)

        self.sectionZdir_fmt = f'{self.sectionindex:04d}{self.sectiondirX}'
        self.piecedir_fmt = f'{self.pieceindex:03d}'

        self.rows = grepKeyValueFromFile(self.imghdr, 'samples')
        self.cols = grepKeyValueFromFile(self.imghdr, 'lines')
        self.mins = grepKeyValueFromFile(self.imghdr, 'bands')

        if VERBOSE:
            self.printInfo()

    def printInfo(self):
        print(f'borehole: {self.borehole}')
        print(f'img: {self.imgpath}')
        print(f'hdr: {self.imghdr}')
        print(f'nrows: {self.rows}  ncols: {self.cols}  nmins: {self.mins}')



def print_current_image_info_log(id,borehole,imgptr):
        print(
            id,
            borehole,
            imgptr.imgdir,
            imgptr.filename,
            imgptr.boreholedir,
            imgptr.sectiondir,
            imgptr.sectionZdir_fmt,
            imgptr.piecedir_fmt,
            imgptr.sectionindex,
            imgptr.pieceindex,
            imgptr.rows,
            imgptr.cols,
            imgptr.mins,
            sep=',',
        )


def format_product_base(borehole, imgptr, mindir, reducefactor):
    return ''.join([
        borehole,
        '_',
        imgptr.sectionZdir_fmt,
        '_',
        imgptr.piecedir_fmt,
        '_',
        mindir,
        '.',
        MINSET_ID,
        '.factor_1to',
        f'{reducefactor:03d}',
    ])



def toPGM(imgptr, n, reducefactor, options,infile,outfile,append=False):
    args=[
        ["-width", str(imgptr.rows)],
        ["-height", str(imgptr.cols)],
        ["-band", str(n)],
        ["-factor", str(reducefactor)],
        ["-quiet"]
    ]
    args.extend([o] for o in options)
    runCommand('binaryDataToPGM',args,infile,outfile,append)



def process_borehole_image(borehole, borehole_dest, imgptr, img_path, count):
    borehole_img_dst = os.path.join(borehole_dest,
                                    imgptr.sectionZdir_fmt,
                                    imgptr.piecedir_fmt
    )
    makeDir(borehole_img_dst)

    if LOG:
        print_current_image_info_log(count,borehole,imgptr)

    # loop through reduction factors (simplified to RAW)
    for reducefactor in reductionTargetFactors:
        if DEBUG:
            print('3<<<', imgptr.rows, imgptr.cols, reducefactor)
            w = imgptr.rows // reducefactor
            h = imgptr.cols // reducefactor

        ABUNDANCE_LOCAL = 0
        ABUNDANCE_GLOBAL = 0
        PRESENCE = 0

        if reducefactor == 1:
            ABUNDANCE_LOCAL = 1
            ABUNDANCE_GLOBAL = 1

        minstotarget = range(imgptr.mins)

        for n in minstotarget:
            if DEBUG:
                print(f'4<<< {n}/{imgptr.mins}')

            mindir = f'{n:02d}'
            productbase= format_product_base(borehole, imgptr, mindir, reducefactor)

            minmax_csv_file = f'{borehole}.MINERALS-META.csv'
            json_file  = productbase + '.json'
            pgm_file   = productbase + '.pgm'
            #png_file   = productbase + '.png'
            #alpng_file = productbase + '.abundance.local.png'
            #agpng_file = productbase + '.abundance.global.png'

            dest_dir = os.path.join(borehole_dest,
                                    imgptr.sectionZdir_fmt,
                                    imgptr.piecedir_fmt,
                                    mindir)

            makeDir(dest_dir)

            if WRITEMINMAX:
                append= True
                topgmargs=['-float','-minmaxcsv']
                product_file=os.path.join(borehole_dest,minmax_csv_file)
                toPGM(imgptr,n,reducefactor,topgmargs,img_path, product_file, append)

            if WRITEPRESENCEJSON:
                append= False
                topgmargs=['-onoff','-json']
                product_file=os.path.join(dest_dir, json_file)
                toPGM(imgptr,n,reducefactor,topgmargs,img_path, product_file, append);

            if WRITEPRESENCEPGM:
                append= False
                topgmargs=['-onoff','-pgm']
                product_file=os.path.join(dest_dir, json_file)
                toPGM(imgptr,n,reducefactor,topgmargs,img_path, product_file, append);

            if WRITEJSON:
                # NOT IMPLEMENTED : TOO SPACE CONSUMING
                pass

            if WRITEPGM:
                append= False
                topgmargs=['-float','-pgm']
                minbandv=0
                maxbandv=1
                topgmargs.append(['-databounds', str(minbandv), str(maxbandv)])
                if len(EHANCEFACTORS)>0:
                    for e in EHANCEFACTORS:
                        topgmargsE=topgmargs+['-enhancement', e]
                        pgm_e_file=productbase+'.e_'+e+'.pgm'
                        product_file=os.path.join(dest_dir, pgm_e_file)
                        toPGM(imgptr,n,reducefactor,topgmargsE,img_path, product_file, append);
                else:
                    product_file=os.path.join(dest_dir, pgm_file)
                    toPGM(imgptr,n,reducefactor,topgmargs,img_path, product_file, append);
        



def process_borehole(borehole):
    count = 0
    if DEBUG:
        print('1<', borehole)

    borehole_dest = os.path.join(DEST_DIR, borehole)
    borehole_src = os.path.join(DATA_DIR, borehole)

    makeDir(borehole_dest)

    if WRITEMINMAX:
        meta_path = os.path.join(borehole_dest, f'{borehole}.MINERALS-META.csv')
        with open(meta_path, 'w') as f:
            f.write('MINERAL,MIN,MAX\n')

    if VERBOSE:
        print(f'{borehole_src}')

    os.chdir(borehole_src)
    for img_path in glob.glob(f'*/*{MINERAL_SET_TARGET}.img'):
        imgptr = ImageHandler(borehole, img_path)
        process_borehole_image(borehole, borehole_dest, imgptr, img_path, count)
        count=count+1
       


def processMinerals(args):
    global DEBUG
    global DRYRUN
    global VERBOSE
    global WRITEMINMAX
    global WRITEJSON
    global WRITEPGM
    global WRITEPRESENCEPGM
    global WRITEPRESENCEJSON
    global LOG
    global BOREHOLES
    global REDUCEFACTORS
    global BANDS
    global EHANCEFACTORS

    i = 1
    while i < len(args):
        if args[i] == '-h' or args[i] == '-help':
            print( args[0],'[-h|-help]')
            print('         [-LOG] [-DEBUG] [-DRYRUN] [-VERBOSE]')
            print('         [-minmax]')
            print('         [-reducefactors n rf1 rf2 ... rfn]')
            print('         [-boreholes n b1 b2 ... bn]')
            print('         [-bands m band1 band2 ... bandm]')  
            return
        elif args[i] == '-LOG':
            LOG = True
        elif args[i] == '-DEBUG':
            DEBUG = True
        elif args[i] == '-DRYRUN':
            DRYRUN = True
        elif args[i] == '-VERBOSE':
            VERBOSE = True
        elif args[i] == '-minmax' or args[i] == '-minmaxcsv':
            WRITEMINMAX = True

        elif args[i] == '-boreholes':
            i = i + 1
            nboreholes = int(args[i])
            BOREHOLES = []
            for j in range(nboreholes):
                i = i + 1
                BOREHOLES.append(args[i])

        elif args[i] == '-bands':
            i = i + 1
            nbands = int(args[i])
            BANDS = []
            for j in range(nbands):
                i = i + 1
                BANDS.append(args[i])

        elif args[i] == '-enhancefactors':
            i = i + 1
            nenhancefactors = int(args[i])
            EHANCEFACTORS=[]
            for j in range(nenhancefactors):
                i = i + 1
                EHANCEFACTORS.append(args[i])

        elif args[i] == '-reducefactors':
            i = i + 1
            nrf = int(args[i])
            REDUCEFACTORS = []
            for j in range(nrf):
                i = i + 1
                REDUCE_FACTORS.apppend(int(args[i]))

        i = i + 1

    for b in BOREHOLES:
        process_borehole(b)


if __name__ == '__main__':
    processMinerals(sys.argv)
