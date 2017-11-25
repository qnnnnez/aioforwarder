#pragma once
#include <memory>
#include <queue>
#include <iostream>
#include <tuple>
#include "asio.h"

class session : public std::enable_shared_from_this<session>
{
public:
	explicit session(tcp::socket src, tcp::socket dst);
	virtual ~session();
	void start();

protected:
	void src_read_some_handler(error_code ec, std::size_t length);
	void dst_read_some_handler(error_code ec, std::size_t length);
	void src_write_handler(error_code ec, std::size_t);
	void dst_write_handler(error_code ec, std::size_t);
	bool enqueue_src_write(std::size_t length);
	bool enqueue_dst_write(std::size_t length);
	void do_src_read();
	void do_dst_read();
	void do_src_write();
	void do_dst_write();

	tcp::socket src_socket, dst_socket;

	static const std::size_t read_buffer_length = 1024 * 10;
	char src_read_buffer[read_buffer_length], dst_read_buffer[read_buffer_length];
	std::size_t write_buffer_length = read_buffer_length;

	struct write_buffer_info
	{
		void *buffer;
		std::size_t length;
	};
	std::queue<write_buffer_info> src_write_queue, dst_write_queue;

	const std::size_t write_buffer_max = 1024 * 1024;
	bool src_writing = false, dst_writing = false, src_reading = false, dst_reading = false;
	std::size_t src_buffer_length_sum = 0, dst_buffer_length_sum = 0;
};