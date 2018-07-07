#!/bin/sh

DIR=.
SCRIPT=./gh-md-toc

if [ ! -z $1 ] && [ -d $1 ] ; then
  DIR=$1
fi

if [ ! -z $2 ] && [ -x $2 ] ; then
  SCRIPT=$2
elif [ ! -x $SCRIPT ] ; then
  wget https://raw.githubusercontent.com/ekalinin/github-markdown-toc/master/gh-md-toc
  chmod a+x gh-md-toc
fi

if [ ! -z $1 ] && ( [ "$1" = "-h" ] || [ "$1" = "--help" ] ) ; then
  echo "Usage for $0 <dir> <script>:"
  echo "  <dir>:    directory to generate tocs for. Best to use a checkout"
  echo "            of a github wiki repo. This can be done with:"
  echo ""
  echo "            git clone https://www.github.com/jredmondson/madara.wiki.git"
  echo ""
  echo "  <script>: the location of gh-md-toc. If not provided, will be downloaded"
  echo ""
  exit 0
fi

for md in $DIR/*.md; do
    echo "Generating toc for $md to $md.toc"
    $SCRIPT $md > $md.toc
done

