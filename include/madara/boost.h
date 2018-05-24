#ifndef MADARA_BOOST_H_
#define MADARA_BOOST_H_

//! This file includes all boost headers we use, wrapping them to suppress warnings

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define BOOST_SYSTEM_NO_DEPRECATED 1

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
