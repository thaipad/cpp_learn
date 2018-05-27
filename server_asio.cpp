#include <iostream>
#include <cstdlib>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

//using boost::asio::ip::tcp;

class session {
private:
	boost::asio::ip::tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
public:
	session(boost::asio::io_service &io_service): socket_(io_service) {}
	boost::asio::ip::tcp::socket &socket() {return socket_;}
	void start() {
		socket_.async_read_some(
			boost::asio::buffer(data_, max_length), 
			boost::bind(&session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	void handle_read(const boost::system::error_code &error, size_t bytes_transferred) {
		if (!error) {
			boost::asio::async_write(
					socket_, 
					boost::asio::buffer(data_, bytes_transferred), 
					boost::bind(&session::handle_write, this, boost::asio::placeholders::error)); 
		} else {
			delete this;
		}	
	}
	void handle_write(const boost::system::error_code &error) {
		if (!error) {
			socket_.async_read_some(
				boost::asio::buffer(data_, max_length), 
				boost::bind(&session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		} else {
			delete this;
		}	
	}
};

class server {
private:
	boost::asio::io_service &io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
public:
	server(boost::asio::io_service &io_service, char ip_addr[], short port):
			io_service_(io_service),
			acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip_addr), port)) {
		session *new_session = new session(io_service_);
		acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept,this, new_session, boost::asio::placeholders::error));
	}	
	void handle_accept(session *new_session, const boost::system::error_code &error) {
		if (!error) {
			new_session->start();
			new_session = new session(io_service_);
			acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept,this, new_session, boost::asio::placeholders::error));
		} else {
			delete this;
		}	
	}
};


int main(int argc, char **argv) {
	boost::asio::io_service io_service;
	server serv(io_service, argv[1], atoi(argv[2]));
	io_service.run();	
	return 0;
}
