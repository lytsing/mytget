#!/bin/sh

# Initial variables
topdir=$1
name=$2
version=$3
release=$4

# Echo shell commands
set -x

# Change to topdir
cd $topdir

# Verify and adjust $name/$version variables
spec_name=`cat $topdir/rpm/mytget.spec |
	grep -E "^Name:" |
	sed -n -e 's/: \?/ /' -e 's/ \+/:/gp' |
	awk -F ':' '{print $2}'`

spec_version=`cat $topdir/rpm/mytget.spec |
	grep -E "^Version:" |
	sed -n -e 's/: \?/ /' -e 's/ \+/:/gp' |
	awk -F ':' '{print $2}'`

if [ $name != $spec_name ]
then
    echo "Warning: name is unexpected! Use spec_name instead."
    name=$spec_name
fi

if [ $version != $spec_version ]
then
    echo "Warning: version is unexpected! Use spec_version instead."
    version=$spec_version
fi

# Create rpm source: $name-$version.tar.gz
project=$name-$version
rm -rf $project.tar.gz $project
test -d .git && git clone . $project || svn export . $project
rm -rf $project/autom4te.cache $project/.git
tar -czf $project.tar.gz $project && rm -rf $project

# Let's go to create rpm
cd $topdir/rpm
rpm_create $name.spec -v $version -r $release -p /usr/local/bin -k
#rpmbuild -ba $name.spec
