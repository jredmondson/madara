#ifndef MADARA_BOOST_H_
#define MADARA_BOOST_H_

//! This file includes all boost headers we use, wrapping them to suppress
//! warnings

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#elif _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif  // _WIN32_WINNT
#endif  // _WIN32

#define BOOST_SYSTEM_NO_DEPRECATED 1

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

#endif  // MADARA_BOOST_H_