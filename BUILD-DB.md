# core-inspector-tools : BUILD-DB
# by santiago lombeyda & jeff brewer (2024)

here are the steps to rebuilding the complete data-www database for core-inspector

STEP 0 (optional)
===================================
# this step cleans the builds,
# deletes all data
# rebuilds executables
make clean
make purge-data
make all

STEP 1
===================================
# this step creates a top meta.js
scripts/makeMineralThumbnails.csh

STEP 2
===================================
# this steps builds the directory
# structure, and creates mineral
# images as pngs and jsons
scripts/processMinerals.csh

STEP 3
===================================
# this step builds the directory
# structure (if not built already)
# and creates spectra json files
# at expected resolution
scripts/processSpectra.csh
