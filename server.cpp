#include "server.h"
#include "session.h"


server::server(boost::asio::io_service &io_service, unsigned short port) :
	acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
	socket_(io_service)
{
	do_accept();
}

server::~server()
{
}

void server::do_accept()
{
	acceptor_.async_accept(socket_, [this](boost::system::error_code ec)
	{
		if (!ec)
		{
			auto remote = get_remote();
			if (remote.port() == 0)
			{
				write_and_close(std::move(socket_), boost::asio::buffer("?????", 5));
			}
			else
			{
				do_connect(std::move(socket_), remote);
			}
		}
		do_accept();
	});
}

void server::do_connect(tcp::socket src_, tcp::endpoint remote)
{
	auto src = new tcp::socket(std::move(src_));
	auto dst = new tcp::socket(make_connect_socket());
	dst->async_connect(remote, [src, dst](error_code ec)
	{
		if (ec)
		{
			src->close();
		}
		else
		{
			auto s = std::make_shared<session>(std::move(*src), std::move(*dst));
			s->start();
		}
		delete src;
		delete dst;
	});
}

tcp::socket server::make_connect_socket()
{
	return tcp::socket(io_service());
}

boost::asio::io_service& server::io_service()
{
	return socket_.get_io_service();
}

template<typename ConstBufferSequence>
void write_and_close(tcp::socket socket_, const ConstBufferSequence data)
{
	auto socket = new tcp::socket(std::move(socket_));
	boost::asio::async_write(*socket, data, [socket](error_code ec, std::size_t length)
	{
		socket->close();
		delete socket;
	});
}
