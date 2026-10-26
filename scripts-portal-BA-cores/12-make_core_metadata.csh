#!/bin/csh

# ./make_core_metadata.csh _BA1A > _BA1A/core-metadata.json 
# ./make_core_metadata.csh _BA3A > _BA3A/core-metadata.json

# set CORE = ARGS[1]
# @ n = `ls -d $CORE/fullscale/0* | wc -l`

@ n = `ls -d _BA3A/fullscale/0* | wc -l`

printf '{"topDepth":0.0,"bottomDepth":400.0,'
printf '"numSection":'$n'","partIds":['

ls -d _BA4A/fullscale/????/??? | sed -e 's/.*fullscale/"/' -e 's_/0*__' -e 's_/0*_\__' -e 's/$/"/' | \
	awk 'BEGIN{entry="";}{ if (ENTRY!="") printf ("%s,",ENTRY); ENTRY=$1;}END{printf ENTRY;}'
echo "]}"
