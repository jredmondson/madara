#!/bin/sh

cd $MADARA_ROOT
mvn -f port/java versions:set -DnewVersion=`cat VERSION.txt`
mvn -f port/java -P release clean package deploy
