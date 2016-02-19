#ifndef  _MADARA_VARIABLE_REFERENCE_INL_
#define  _MADARA_VARIABLE_REFERENCE_INL_


/**
 * @file VariableReference.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the inlined functions for the VariableReference class
 */

inline const char *
madara::knowledge::VariableReference::get_name (void) const
{
  return name_.get_ptr ();
}

inline void
madara::knowledge::VariableReference::set_name (
const std::string & name)
{
  // create a new char array and copy over the string
  uint32_t size = uint32_t (name.length () + 1);
  char * temp = new char[size];
  strncpy (temp, name.c_str (), size - 1);
  temp[size - 1] = 0;
  name_ = temp;
}

inline bool
madara::knowledge::VariableReference::is_valid (void) const
{
  return record_ != 0;
}



#endif   // _MADARA_VARIABLE_REFERENCE_INL_
