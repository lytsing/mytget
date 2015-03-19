#!/bin/sh

# Initial variables
topdir=$1
name=$2
version=$3
release=$4

# Echo shell commands
set -x

cd $topdir/rpm

if [ `uname -i` == "x86_64" ]
then
    plat="x86_64"
else
    plat="i386"
fi

for name in `ls *.rpm`
do
    yum-upload $name  --osver $ABS_OS --arch ${plat} --group yum --batch
done
