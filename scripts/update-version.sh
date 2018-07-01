#!/bin/bash
# For some reason we have to set working and repository folder explicitly
# https://stackoverflow.com/questions/11914933/fatal-not-a-git-repository-git-error

# Get version number (according to Semantic versioning, based on last tag),
# number of commits after last tag and hash of current commit
VERSION=`git --git-dir .git --work-tree . describe --tags --long --always`

# Add current branch to VERSION
VERSION="$VERSION-`git rev-parse --abbrev-ref HEAD`"

VERSION_FILE=utils/version.cpp

echo "Updating version to $VERSION in file $VERSION_FILE"
cp $VERSION_FILE.tmpl $VERSION_FILE
sed --in-place -e "s/^const char\* const version = \".*\";$/const char\* const version = \"$VERSION\";/g" $VERSION_FILE
