########### INTRODUCTION ##############

In this directory, we have provided some simple karl logics that will help
show how to use the language for real-time checking of state with the karl
interpreter or that may be loaded from custom MADARA applications.

########## DISTANCE CHECK #############

The distance_check.karl logic computes the distance between two positions
and then determines .mission_success according to two types of possible
missions: a) the positions need to be closer than a .minimum_distance or 
b) the positions need to be farther than a .minimum_distance. Two example
files are provided for seeing how to configure and use distance_check.karl.

  ###WINDOWS#####

  // check for a buffer distance (.pos1 needs to be farther out than .minimum_distance)
  %MADARA_ROOT%\bin\karl -0f %MADARA_ROOT%\examples\logics\check_for_buffer_distance.karl -i %MADARA_ROOT%\examples\logics\distance_check.karl -k

  // check for a closer distance (.pos1 needs to be closer than .minimum_distance)
  %MADARA_ROOT%\bin\karl -0f %MADARA_ROOT%\examples\logics\check_for_minimum_distance.karl -i %MADARA_ROOT%\examples\logics\distance_check.karl -k
  
  ###LINUX#####

  // check for a buffer distance (.pos1 needs to be farther out than .minimum_distance)
  $MADARA_ROOT/bin/karl -0f $MADARA_ROOT/examples/logics/check_for_buffer_distance.karl -i $MADARA_ROOT/examples/logics/distance_check.karl -k

  // check for a closer distance (.pos1 needs to be closer than .minimum_distance)
  $MADARA_ROOT/bin/karl -0f $MADARA_ROOT/examples/logics/check_for_minimum_distance.karl -i $MADARA_ROOT/examples/logics/distance_check.karl -k
  
  