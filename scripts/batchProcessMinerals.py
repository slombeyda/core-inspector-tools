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

MINERAL_SET_TARGET_POOL = ['SWIRcalib_map_export', 'VNIRcalib_resampled_map_export']
MINSET_ID_POOL          = ['SWIR', 'VNIR']

MINERAL_SET_TARGET = 'VNIRcalib_resampled_map_export'
MINSET_ID = 'VNIR'

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

ENHANCEFACTORS = []

reductionTargets = ['RAW']
reductionTargetFactors = [1]

ABUNDANCE_REFERENCE_FILE = ''
ABUNDANCE_PRESET_MIN = 0.0
ABUNDANCE_PRESET_MAX = 1.0
USEABUNDANCEFILE = False

JSON_EXTENSION = 'json'
PGM_EXTENSION  = 'pgm'

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


def grepKeyValueFromFile(filepath, key, delimiter='=', nth=1):
    with open(filepath) as f:
        for line in f:
            if line.strip().startswith(key):
                try:
                    return line.split(delimiter)[nth]
                except Exception:
                    return 0
    return 0


def grepKeyIntFromFile(filepath, key, delimiter='=', nth=1):
    try:
        return int(grepKeyValueFromFile(filepath, key, delimiter, nth))
    except Exception:
        return 0   


def grepKeyFloatFromFile(filepath, key, delimiter='=', nth=1):
    try:
        return float(grepKeyValueFromFile(filepath, key, delimiter, nth))
    except Exception:
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

        self.rows = grepKeyIntFromFile(self.imghdr, 'samples')
        self.cols = grepKeyIntFromFile(self.imghdr, 'lines')
        self.mins = grepKeyIntFromFile(self.imghdr, 'bands')

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
        ["-width",  str(imgptr.rows)],
        ["-height", str(imgptr.cols)],
        ["-band",   str(n)],
        ["-factor", str(reducefactor)],
        ["-quiet"]
    ]
    args.append(options)
    runCommand('binaryDataToPGM',args,infile,outfile,append)


def process_borehole_image(borehole, borehole_dest, imgptr, img_path, count):
    global BANDS
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

        if BANDS==[]:
            nminstotarget = imgptr.mins
            BANDS = range(nminstotarget)
        else:
            nminstotarget = len(BANDS)

        for nm in range(nminstotarget):
            b=int(BANDS[nm])
            if DEBUG:
                print(f'4<<< {b}/{imgptr.mins}')

            mindir = f'{b:02d}'
            productbase= format_product_base(borehole, imgptr, mindir, reducefactor)

            minmax_csv_file = f'{borehole}.{MINSET_ID}.MINERALS-META.csv'
            json_file  = productbase + f'.{JSON_EXTENSION}'
            pgm_file   = productbase + f'.{PGM_EXTENSION}'
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
                toPGM(imgptr,b,reducefactor,topgmargs,img_path, product_file, append)

            if WRITEPRESENCEJSON:
                append= False
                topgmargs=['-onoff','-json']
                product_file=os.path.join(dest_dir, json_file)
                toPGM(imgptr,b,reducefactor,topgmargs,img_path, product_file, append);

            if WRITEPRESENCEPGM:
                append= False
                topgmargs=['-onoff','-pgm']
                product_file=os.path.join(dest_dir, json_file)
                toPGM(imgptr,b,reducefactor,topgmargs,img_path, product_file, append);

            if WRITEJSON:
                # NOT IMPLEMENTED : TOO SPACE CONSUMING
                pass

            if WRITEPGM:
                append= False
                topgmargs=['-float','-pgm']

                if USEABUNDANCEFILE:
                    minv=grepKeyFloatFromFile(ABUNDANCE_REFERENCE_FILE, str(b)+',', ',', 1)
                    maxv=grepKeyFloatFromFile(ABUNDANCE_REFERENCE_FILE, str(b)+',', ',', 2)
                else:
                    minv=str(ABUNDANCE_PRESET_MIN)
                    maxv=str(ABUNDANCE_PRESET_MAX)
                topgmargs.extend(['-databounds', minv, maxv])

                if len(ENHANCEFACTORS)>0:
                    for e in ENHANCEFACTORS:
                        topgmargsE=topgmargs+['-enhancement', e]
                        pgm_e_file=productbase+'.e_'+e+'.pgm'
                        product_file=os.path.join(dest_dir, pgm_e_file)
                        toPGM(imgptr,b,reducefactor,topgmargsE,img_path, product_file, append);
                else:
                    product_file=os.path.join(dest_dir, pgm_file)
                    toPGM(imgptr,b,reducefactor,topgmargs,img_path, product_file, append);


def process_borehole(borehole):
    count = 0
    if DEBUG:
        print('1<', borehole)

    borehole_dest = os.path.join(DEST_DIR, borehole)
    borehole_src = os.path.join(DATA_DIR, borehole)

    makeDir(borehole_dest)

    if WRITEMINMAX:
        meta_path = os.path.join(borehole_dest, f'{borehole}.MINERALS-META.{MINSET_ID}.csv')
        with open(meta_path, 'w') as f:
            f.write('MINERAL,MIN,MAX\n')

    if VERBOSE:
        print(f'{borehole_src}')

    os.chdir(borehole_src)
    for img_path in glob.glob(f'*/*{MINERAL_SET_TARGET}.img'):
        imgptr = ImageHandler(borehole, img_path)
        process_borehole_image(borehole, borehole_dest, imgptr, img_path, count)
        count=count+1


def printDefaults():
    print("MINERAL_SET_TARGET:", MINERAL_SET_TARGET)
    print("MINSET_ID:", MINSET_ID)
    print("BOREHOLES:", BOREHOLES)
    print("BANDS:", BANDS)
    print("REDUCEFACTORS:", REDUCEFACTORS)
    print("ENHANCEFACTORS:", ENHANCEFACTORS)
    print("ABUNDANCE_PRESET_MIN:", ABUNDANCE_PRESET_MIN)
    print("ABUNDANCE_PRESET_MAX:", ABUNDANCE_PRESET_MAX)

    print("MINERAL_SET_TARGET_POOL:", MINERAL_SET_TARGET_POOL)
    print("MINSET_ID_POOL:", MINSET_ID_POOL)

def printUsage(args):
    print(f'{args[0]} [-h|-help]')
    print('         [-LOG] [-DEBUG] [-DRYRUN] [-VERBOSE]')
    print('         [-extension ext] [-jsonextension ext] [-pgmextension ext]')
    print('         [-minmax|-minmaxcsv] [-pgm|-writepgm] [-writepresencepgm] [-writepresencejson]')
    print('         [-reducefactors n rf1 rf2 ... rfn]')
    print('         [-boreholes n b1 b2 ... bn]')
    print('         [-bands m band1 band2 ... bandm]')
    print('         [-mineralsetfilename filename]')
    print('         [-mineralsetshortid shortid]')
    print('         [-usenthmineralset n] [-usemineralsetbyid setid]')
    print('         [-abundancerange min max]')
    print('         [-abundancereferencefile filename]')
    print('         [-enhancefactors p ef1 ef2 ... efp]')
    print()

def processMinerals(args):

    global LOG
    global DEBUG
    global DRYRUN
    global VERBOSE
    global WRITEMINMAX
    global WRITEJSON
    global WRITEPGM
    global WRITEPRESENCEPGM
    global WRITEPRESENCEJSON

    global BOREHOLES
    global REDUCEFACTORS
    global BANDS
    global ENHANCEFACTORS
    global MINERAL_SET_TARGET
    global MINSET_ID

    global ABUNDANCE_PRESET_MIN
    global ABUNDANCE_PRESET_MAX
    global USEABUNDANCEFILE
    global ABUNDANCE_REFERENCE_FILE

    global JSON_EXTENSION
    global PGM_EXTENSION

    i = 1
    while i < len(args):
        if args[i] == '-h' or args[i] == '-help':
            printUsage(args)
            return
        if args[i] == '-defaults':
            printDefaults()
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

        elif args[i] == '-writepresencepgm':
            WRITEPRESENCEPGM = True

        elif args[i] == '-writepresencejson':
            WRITEPRESENCEJSON = True

        elif args[i] == '-pgm' or args[i] == '-writepgm':
            WRITEPGM = True

        elif args[i] == '-extension':
            i = i + 1
            JSON_EXTENSION = args[i]
            PGM_EXTENSION = args[i]

        elif args[i] == '-jsonextension':
            i = i + 1
            JSON_EXTENSION = args[i]

        elif args[i] == '-pgmextension':
            i = i + 1
            PGM_EXTENSION = args[i]

        elif args[i] == '-abundancerange':
            i = i + 1
            ABUNDANCE_PRESET_MIN = float(args[i])
            i = i + 1
            ABUNDANCE_PRESET_MAX = float(args[i])

        elif args[i] == '-abundancereferencefile':
            i = i + 1
            ABUNDANCE_REFERENCE_FILE = args[i]
            USEABUNDANCEFILE = True

        elif args[i] == '-usenthmineralset':
            i = i + 1
            nth = int(args[i])
            if nth < len(MINERAL_SET_TARGET_POOL):
                MINERAL_SET_TARGET = MINERAL_SET_TARGET_POOL[nth]
                MINSET_ID = MINSET_ID_POOL[nth]
            else:
                print(f'Error: -usenthmineralset {nth} out of range')
                return

        elif args[i] == '-usemineralsetbyid':
            i = i + 1
            setid = args[i]
            if setid in MINSET_ID_POOL:
                index = MINSET_ID_POOL.index(setid)
                MINERAL_SET_TARGET = MINERAL_SET_TARGET_POOL[index]
                MINSET_ID = MINSET_ID_POOL[index]
            else:
                print(f'Error: -usemineralsetbyid {setid} not found')
                return

        elif args[i] == '-mineralsetfilename':
            i = i + 1
            MINERAL_SET_TARGET = args[i]

        elif args[i] == '-mineralsetfilename':
            i = i + 1
            MINERAL_SET_TARGET = args[i]

        elif args[i] == '-mineralsetshortid':
            i = i + 1
            MINSET_ID = args[i]

        elif args[i] == '-boreholes':
            i = i + 1
            nboreholes = int(args[i])
            BOREHOLES = []
            for j in range(nboreholes):
                i = i + 1
                BOREHOLES.append(args[i])

        elif args[i] == "-bands":
            i = i + 1
            nbands = int(args[i])
            BANDS = []
            for j in range(nbands):
                i = i + 1
                BANDS.append(args[i])

        elif args[i] == '-enhancefactors':
            i = i + 1
            nenhancefactors = int(args[i])
            ENHANCEFACTORS=[]
            for j in range(nenhancefactors):
                i = i + 1
                ENHANCEFACTORS.append(args[i])

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
