/*
*		@authors Jake Pitkin, Daniel Rushton
*		Network connection object that encapsulates all processes involved with 
*		connecting to and sending/recieving messages to a spreadsheet
*		server instance.
*/

#include "connection.h"

/*
 * constructs a new connection object with the specififed 
 * hostname and port. 
 */

connection::connection(std::string hostname, std::string port)
{
  // Resolve the ip of the host name
  ip_address = resolve_host(hostname);
  connected_port = port;

  // instantiate incoming/outgoing queues
  outgoing = new safe_queue();
  incoming = new safe_queue();

  // instantiate thread group
  threads = new boost::thread_group();

  connect();
}

/*
 * destructor, calls cleanup method to clean up all connection
 * resources before the object is destroyed.
 */

connection::~connection() {}

/*
 * creates connection by binding a socket with the specified
 * server, spawns the incoming and outgoing threads, allocates
 * the incoming and outgoing queues.
 */


// helper function to convert string to integer 
int string_to_int(const std::string& str)
{
    int result;
    std::istringstream ss(str);
    ss.imbue(std::locale::classic());
    ss >> result;
    return result;
}

// attempts to connect to specified host, returns true if a connection was established
bool connection::connect()
{
  try 
  {
  	bound = false;
        // attempt to establish a connection
	boost::asio::io_service service;
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip_address), string_to_int(connected_port));
	socket_ptr s(new boost::asio::ip::tcp::socket(service));
	socket = s;
	socket->connect(ep);

	running = true;
	bound = true;

	threads->create_thread(boost::bind(&connection::send_loop, this));
	threads->create_thread(boost::bind(&connection::receive_loop, this));

	return true;

  }
  catch(std::exception& e)
  {
    std::cout << "Unable to connect to server! " <<  e.what() << std::endl;
    return false;
  }

    return true;
}

#include <string>
#include <sstream>
 





/*
 * returns true if this connection instance is bound to 
 * a server.
 */

bool connection::is_bound()
{
  return bound;
}


/*
 * shuts down this instance of connection and 
 * cleans up all of its resources.
 */

void connection::close()
{
  cleanup();
}


/*
 *  takes a string as a parameter and enqueue it 
 *  on the outgoing queue.
 */

void connection::send_message(std::string msg)
{
	outgoing->push(msg);
}



/*
 * attempts to dequeue the next pending message from 
 * the incoming queue. returns the message if it exists
 * returns a null string otherwise
 */

std::string connection::receive_next()
{
	std::string msg;

	if (incoming->try_pop(msg))
	{
		return msg;
	}

	return "";
}


/* 
 *  cleans up pointers, sockets, and threads when closing
 *  a connection object or prior to the connection object 
 *  being destroyed.
 */

void connection::cleanup()
{
  running = false;
  threads->join_all();

  socket.reset();
  delete outgoing;
  delete incoming;
}




/* 
 * repeatedly checks the send queue for pending messages
 * when a message is present, appends the /n character and
 * sends the message down the socket.
 */	
void connection::send_loop()
{
	const int buff_size = 1024;
	char buff[buff_size] = {0};
	std::string msg;

	while(running)
	{
		if ( outgoing->try_pop(msg) )
		{
			msg += '\n';

			try
			{
			  socket->write_some(boost::asio::buffer(msg, buff_size));
			}
			catch(std::exception& e)
			{
				bound = false;
			}

			msg.clear();
			memset(buff, 0, buff_size);
		}
	}
}



/*
 * repeatedly checks the socket for incoming messages.
 * appends pieces of messages to a string until a /n is 
 * encountered, and then enques the completed message.
 */
void connection::receive_loop()
{
	const int buff_size = 1024;
	int bytes = 0;
	char buff[buff_size] = {0};
	char temp_buff[buff_size] = {0};
	std::string msg;

	while(running)
	{
		if(socket->available())
		{
			try
			{
			  bytes = socket->read_some(boost::asio::buffer(buff, buff_size));
			}
			catch(std::exception& e)
			{
				bound = false;
			}
			

			for(int i = 0; i < buff_size; i++)
			{
				if(buff[i] == '\n')
				{
					incoming->push(msg);
					msg.clear();
				}
				else
				{
					msg += buff[i];
				}
			}

 			memset(buff, 0, 1024);

		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(50));

	}
}


bool connection::has_next()
{
  return !incoming->empty(); 
}




/*
 * Resolves a hostname to its IP address. Returns null if
 * the hostname cannot be resolved.
 */
std::string connection::resolve_host(std::string hostname)
{
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::query query(hostname, "");
  for(boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(query);
    i != boost::asio::ip::tcp::resolver::iterator(); ++i)
  {
    boost::asio::ip::tcp::endpoint end = *i;
    // Return the resolved IP address
    return end.address().to_string();
  }

  return NULL;
}

// safe_queue implementation

void connection::safe_queue::push(std::string const& data)
{
    boost::mutex::scoped_lock lock(the_mutex);
    backing_queue.push(data);
    lock.unlock();
    //cond.notify_one();
}

bool connection::safe_queue::empty() const
{
    boost::mutex::scoped_lock lock(the_mutex);
    return backing_queue.empty();
    //lock.unlock();
}

bool connection::safe_queue::try_pop(std::string& popped_value)
{
    boost::mutex::scoped_lock lock(the_mutex);
    if(backing_queue.empty()) 
    { return false; }
    popped_value= backing_queue.front();
    backing_queue.pop();
    return true; 
}


















