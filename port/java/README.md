#### Using Eclipse IDE for Developing MADARA Java port
MADARA Java port is compliant with Maven, so you can either use Eclipse or any other IDE of your choice.

* Download the Eclipse Package "IDE for Java Developers". At the time of writing this was Eclipse Photon IDE link https://www.eclipse.org/downloads/packages/release/photon/r/eclipse-ide-java-developers
* Open Eclipse IDE and create workspace at any location other than MADARA repo
* Go to File -> Import to open the project import wizard. 
* Select "Existing projects into Workspace" and press Next
* Enter project root directory location as `$MADARA_ROOT/port/java`
* Once the wizard is complete the project will be imported in the Workspace.




#### To generate jar files for development purpose

mvn -f port/java versions:set -DnewVersion=`cat VERSION.txt`
mvn -f port/java -P development clean package install

####  To generate release the artifacts for central Maven repository or any internal maven repository:

mvn -f port/java versions:set -DnewVersion=`cat VERSION.txt`
mvn -f port/java -P release clean package deploy
