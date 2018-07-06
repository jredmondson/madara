Welcome to MADARA!                 {#mainpage}
==================

[TOC]

You are accessing the core C++ API documentation for one of the most feature-rich
and fastest distributed application development libraries in the world. MADARA 
has been primarily used for building distributed artificial intelligence and 
knowledge bases in robotics that are resilient to extreme outdoor environments,
network disconnections, and problematic sensors and actuators.

MADARA facilitates development and even formal verification by providing a unique,
intuitive interface that integrates a distributed knowledge base into C++ programs
through the inclusion of a knowledge base, a networking transport, and C++ variables
called containers that look and feel like normal object-oriented C++ code, but that
send and receive knowledge modifications over the network. MADARA includes advanced threading
and networking features like bandwidth enforcement, network filtering, and thread
hertz enforcement. It also includes hundreds of other quality-of-service and optimization
features that allow MADARA to operate fast and efficiently, even on ARM and other reduced
instruction set CPUs.

For new users, you may want to try the C++ guide series, which walks developers through the MADARA API
and basic usage.


___
\section new_users New Users
*C++ Guide Series*: [Architecture](http://www.github.com/jredmondson/madara/wiki/MadaraArchitecture) | [Knowledge Base](http://www.github.com/jredmondson/madara/wiki/InteractingWithTheKnowledgeBase) | [Networking](http://www.github.com/jredmondson/madara/wiki/InteractingWithTheTransport) | [Containers](http://www.github.com/jredmondson/madara/wiki/KnowledgeContainers) | [Threads](http://www.github.com/jredmondson/madara/wiki/WorkingWithThreads) | [Optimizations](http://www.github.com/jredmondson/madara/wiki/OptimizingKaRL) | [KaRL](http://www.github.com/jredmondson/madara/wiki/KarlLanguage/)
___
\section advanced_users Advanced Users

For more advanced users, this API documentation is directly generated from the code base and includes as
much information as our developers can think of to help with creating, debugging, and optimizing your code.
You can use the Namespace and Class list navigations to find specific documentation, or you use the topic
areas below:

![MADARA Architecture](MADARA_Architecture.png)

Places in the API documentation to start looking for information on:

1. Using The Knowledge Base
    1. Browse object-oriented containers via madara::knowledge::containers classes (preferred)
    2. Use direct Knowledge Base functions via madara::knowledge::KnowledgeBase
2. Transfering Knowledge / Networking
    1. Browse the builtin transports for UDP, Broadcast or Multicast in madara::transport
    2. Make your own transport by extending madara::transport::Base
3. Logging information to files or console
    1. Browse verbose logging options for MADARA internals via madara::logger::LogLevels set on madara::logger::Logger via madara::logger::Logger::set_level
    2. Use the global logger (madara::logger::global_logger) by including madara/logger/GlobalLogger.h
    3. Create your own logger and enumerations by creating a new instance of madara::logger::Logger. You don't have to extend it.
    4. Use high performance logging macros like madara::logger::madara_logger_ptr_log with the global_logger or madara::logger::madara_logger_log with your own logger instance. These only perform function calls if the log level is appropriate (super fast)
4. Calling functions from inside the KaRL interpreter
    1. Define custom C++ functions and bind them to function names in KaRL via madara::knowledge::KnowledgeBase::define_function
    2. Evaluate such functions in KaRL by calling madara::knowledge::KnowledgeBase::evaluate
5. Using Event-Based Programming
    1. Create custom on-send events by extending madara::filters::AggregateFilter	and adding them to transports with madara::transport::QoSTransportSettings::add_send_filter(filters::AggregateFilter * filter)
    2. Create custom on-receive events by extending madara::filters::AggregateFilter	and adding them to transports with madara::transport::QoSTransportSettings::add_receive_filter(filters::AggregateFilter * filter)
    2. Create custom on-rebroadcast events by extending madara::filters::AggregateFilter	and adding them to transports with madara::transport::QoSTransportSettings::add_rebroadcast_filter(filters::AggregateFilter * filter)
6. Enforcing Bandwidth Limits
    1. Set bandwidth limits in QoSTransportSettings via madara::transport::QoSTransportSettings::set_send_bandwidth_limit or base limits on total throughput over the transport with madara::transport::QoSTransportSettings::set_total_bandwidth_limit
    2. Create custom filters for madara::transport::QoSTransportSettings::add_send_filter(filters::AggregateFilter * filter) by extending madara::filters::AggregateFilter
7. Creating more realistic tests
    1. Set drop rate and drop type via madara::transport::QoSTransportSettings::update_drop_rate
8. Working with networks
    1. Add built-in transports via madara::knowledge::KnowledgeBase::attach_transport(const std::string &, transport::TransportSettings & settings)
    2. Add custom transports by extending madara::transport::Base and attaching the transport via madara::knowledge::KnowledgeBase::attach_transport(madara::transport::Base *)
     
___


If you have any questions, please contact us through our main website at http://madara.sourceforge.net. Thanks!