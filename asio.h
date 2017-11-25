#pragma once
#define BOOST_ASIO_SEPARATE_COMPILATION
#include <boost/asio.hpp>

using boost::system::error_code;
using boost::asio::ip::tcp;
using boost::asio::mutable_buffer;
using boost::asio::ip::address_v4;