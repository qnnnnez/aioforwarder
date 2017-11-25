#include "session.h"

#define BIND_HANDLER(handler) ([this, self=shared_from_this()](error_code ec, std::size_t length){handler(ec, length);})

session::session(tcp::socket src, tcp::socket dst) :
	src_socket(std::move(src)),
	dst_socket(std::move(dst))
{
	std::cout << "{ create object 0x" << this << std::endl;
}

session::~session()
{
	std::cout << "} delete object 0x" << this << std::endl;
}

void session::start()
{
	src_reading = true;
	do_src_read();

	dst_reading = true;
	do_dst_read();
}

void session::src_read_some_handler(error_code ec, std::size_t length)
{
	if (ec)
	{
		src_socket.close();
		dst_socket.close();
		return;
	}

	if (enqueue_dst_write(length))
	{
		do_src_read();
	}
	else
	{
		src_reading = false;
	}
}

void session::dst_read_some_handler(error_code ec, std::size_t length)
{
	if (ec)
	{
		src_socket.close();
		dst_socket.close();
		return;
	}

	if (enqueue_src_write(length))
	{
		do_dst_read();
	}
	else
	{
		dst_reading = false;
	}
}

void session::src_write_handler(error_code ec, std::size_t)
{
	write_buffer_info used_buffer = src_write_queue.front();
	src_write_queue.pop();
	std::cout << "] deallocating buffer 0x" << used_buffer.buffer << "[" << used_buffer.length << "]" << std::endl;
	boost::asio::asio_handler_deallocate(used_buffer.buffer, used_buffer.length);
	src_buffer_length_sum -= used_buffer.length;
	if (ec)
	{
		src_socket.close();
		dst_socket.close();
		return;
	}

	if (!src_write_queue.empty())
	{
		do_src_write();
	}
	else
	{
		src_writing = false;
	}

	if (!dst_reading)
	{
		dst_reading = true;
		do_dst_read();
	}
}

void session::dst_write_handler(error_code ec, std::size_t)
{
	write_buffer_info used_buffer = dst_write_queue.front();
	dst_write_queue.pop();
	std::cout << "] deallocating buffer 0x" << used_buffer.buffer << "[" << used_buffer.length << "]" << std::endl;
	boost::asio::asio_handler_deallocate(used_buffer.buffer, used_buffer.length);
	dst_buffer_length_sum -= used_buffer.length;

	if (ec)
	{
		src_socket.close();
		dst_socket.close();
		return;
	}

	if (!dst_write_queue.empty())
	{
		do_dst_write();
	}
	else
	{
		dst_writing = false;
	}

	if (!src_reading)
	{
		src_reading = true;
		do_src_read();
	}
}

bool session::enqueue_src_write(std::size_t length)
{
	void *buffer_data = boost::asio::asio_handler_allocate(length);
	std::cout << "[ allocated buffer 0x" << buffer_data << "[" << length << "]" << std::endl;
	std::memcpy(buffer_data, dst_read_buffer, length);

	write_buffer_info info;
	info.buffer = buffer_data;
	info.length = length;
	src_write_queue.push(info);
	src_buffer_length_sum += length;

	if (!src_writing)
	{
		src_writing = true;
		do_src_write();
	}
	return src_buffer_length_sum <= write_buffer_max;
}

bool session::enqueue_dst_write(std::size_t length)
{
	void *buffer_data = boost::asio::asio_handler_allocate(length);
	std::cout << "[ allocated buffer 0x" << buffer_data << "[" << length << "]" << std::endl;
	std::memcpy(buffer_data, src_read_buffer, length);

	write_buffer_info info;
	info.buffer = buffer_data;
	info.length = length;
	dst_write_queue.push(info);
	dst_buffer_length_sum += length;

	if (!dst_writing)
	{
		dst_writing = true;
		do_dst_write();
	}
	return dst_buffer_length_sum <= write_buffer_max;
}

void session::do_src_write()
{
	auto buffer = src_write_queue.front();
	boost::asio::async_write(src_socket, boost::asio::buffer(buffer.buffer, buffer.length), BIND_HANDLER(src_write_handler));
}

void session::do_dst_write()
{
	auto buffer = dst_write_queue.front();
	boost::asio::async_write(dst_socket, boost::asio::buffer(buffer.buffer, buffer.length), BIND_HANDLER(dst_write_handler));
}

void session::do_src_read()
{
	src_socket.async_read_some(boost::asio::buffer(src_read_buffer, read_buffer_length), BIND_HANDLER(src_read_some_handler));
}

void session::do_dst_read()
{
	dst_socket.async_read_some(boost::asio::buffer(dst_read_buffer, read_buffer_length), BIND_HANDLER(dst_read_some_handler));
}