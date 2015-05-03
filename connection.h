/*
*		@authors Jake Pitkin, Daniel Rushton
*		Basic unit testing for connection object.
*/
#include <iostream>
#include <string>
#include <queue>
#include <cstdlib>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifndef CONNECTION_H
#define CONNECTION_H

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

class connection
{
	// forward declaration of safe_queue data structure.
	class safe_queue;

	public:

		// constructs a new connection object with a specified ip address and port.
		connection(std::string ip, std::string port);
		
		// default destructor, cleans up any resources that may still be in use
		// for instance: when the object is destroyed without first calling close.
		~connection();

		// returns true if this connection object has successfully bound to the server.
		bool is_bound();

		// closes this connection and cleans up ALL resources.
		void close();

		// sends a message asynchronously to the connected server.
		void send_message(std::string msg);

		// receives the next message asynchronously from the server.
		std::string receive_next();

		// returns true if there are pending incoming mesages on the queue.
		bool has_next();



	private:

		// ip address of the server we are connected to.
		std::string ip_address;

		// port we are connected on
		std::string connected_port;

		// indicates if the connection should be running
		bool running;

		// indicates if the socket is still bound
		bool bound;

		// queue of messages to send
		safe_queue *outgoing;

		// queue of messages received
		safe_queue *incoming;

		// releases and cleans up any resources in use by this connection.
		void cleanup();

		// connects to the given hostname and ip address.
		bool connect();

		// given a domain name, resolves the respective ip address.
		std::string resolve_host(std::string ip);

		// underlying socket
		socket_ptr socket;

		// group to manage incoming and outgoing threads.
		boost::thread_group *threads;

		// outgoing loop that handles the sending of queued messages.
		void send_loop();

		// incoming loop that processes data from the socket and queues up messages.
		void receive_loop();


		// nested queue data structure class with built in thread-safety
		class safe_queue
		{

			public:

    			void push(std::string const& data);

    			bool empty() const;

    			bool try_pop(std::string& popped_value);

    		private:

    			std::queue<std::string> backing_queue;
   
    			mutable boost::mutex the_mutex;
    
		};

};



#endif 
