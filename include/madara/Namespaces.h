#ifndef _MADARA_NAMESPACES_H_
#define _MADARA_NAMESPACES_H_

/**
 * @file Namespaces.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains namespace documentation. There is no reason to include
 * this file in any project. It is purely for Doxygen documentation.
 **/

/**
 * @namespace madara
 * Contains the MADARA knowledge base and toolset
 **/
namespace madara
{
/**
 * @namespace madara::exceptions
 * Provides exception classes for MADARA
 **/
namespace exceptions
{
}

/**
 * @namespace madara::expression
 * Contains the KaRL expression tree interpreter and related classes
 **/
namespace expression
{
}

/**
 * @namespace madara::filters
 * Provides filtering for the MADARA transport layer
 **/
namespace filters
{
}

/**
 * @namespace madara::knowledge
 * Provides functions and classes for the distributed knowledge base. Main entry
 *point to library.
 **/
namespace knowledge
{
/**
 * @namespace madara::knowledge::containers
 * Provides container classes for fast knowledge base access and mutation
 **/
namespace containers
{
}
}

/**
 * @namespace madara::logger
 * Provides knowledge logging services to files and terminals
 **/
namespace logger
{
}

/**
 * @namespace madara::threads
 * Provides a quality-of-service-enabled threading library
 **/
namespace threads
{
}

/**
 * @namespace madara::transport
 * Provides the network transport layer for knowledge bases to communicate
 *within and across agents
 **/
namespace transport
{
}

/**
 * @namespace madara::utility
 * Provides utility functions and classes for common tasks and needs
 **/
namespace utility
{
/**
 * @namespace madara::utility::java
 * Provides utility functions and classes for common Java tasks and needs
 **/
namespace java
{
}
}
}

#endif  // end _MADARA_NAMESPACES_H_
