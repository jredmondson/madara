To generate jar files for development purpose
mvn -f port/java versions:set -DnewVersion=`cat VERSION.txt`
mvn -f port/java -P development clean package install



To release the artifacts in Maven repository or any internal maven repository:

mvn -f port/java versions:set -DnewVersion=`cat VERSION.txt`
mvn -f port/java -P release clean package deploy
