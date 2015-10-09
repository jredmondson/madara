
#ifdef _MADARA_PYTHON_CALLBACKS_

#ifndef _MADARA_PYTHON_ACQUIRE_GIL_H_
#define _MADARA_PYTHON_ACQUIRE_GIL_H_

#include <boost/python.hpp>

/**
 * @file Acquire_GIL.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains helper classes for Acquiring the GIL.
 **/

namespace madara
{
  namespace python
  {
    /**
     * @class Acquire_GIL
     * @brief This class encapsulates accessing the global interpreter lock
     **/
    class Acquire_GIL 
    {
    public:
      Acquire_GIL ()
      {
        state_ = PyGILStateEnsure();
      }

      ~Acquire_GIL()
      {
        PyGILStateRelease(state_);
      }

    private:
      PyGILState_STATE state_;
    };
  }
}
#endif // not defined _MADARA_PYTHON_ACQUIRE_GIL_H_

#endif // defined _MADARA_PYTHON_CALLBACKS_