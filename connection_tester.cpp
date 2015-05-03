/*
*		@authors Jake Pitkin, Daniel Rushton
*		Basic unit testing for connection object.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <iterator>
#include <string>
#include <queue>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include "connection.h"


// Test success tracking.
bool _all_tests_passed = true;
double _tests_passed = 0.0;
double _tests_failed = 0.0;

// Forward declarations
void test_basic_constructor();
void test_basic();
void test_close();

// ... add test function declarations here ...

// Main entry point.
int main()
{
	std::cout << std::endl << std::endl << std::endl;

	std::cout << "***************************************************" << std::endl;
	std::cout << "               connection Test Suite               " << std::endl;
	std::cout << "***************************************************" << std::endl;
	
	std::cout << std::endl;
	std::cout << "--------------- Testing Constructor ---------------" << std::endl;
	std::cout << std::endl;

	test_basic_constructor();
	
	std::cout << std::endl;
	std::cout << "--------------- Running Basic Tests ---------------" << std::endl;
	std::cout << std::endl;

	test_basic();

	std::cout << std::endl;
	std::cout << "--------------- Testing Edge Cases ----------------" << std::endl;
	std::cout << std::endl;
	
	std::cout << "-------------- Testing Corner Cases ---------------" << std::endl;
	std::cout << std::endl;
	
	std::cout << "------------- Testing Boundary Cases --------------" << std::endl;
	std::cout << std::endl;
	
	std::cout << "------------------ Stress Tests -------------------" << std::endl;
	std::cout << std::endl;

	std::cout << "------------------ Memory Audit -------------------" << std::endl;
	std::cout << std::endl;


	std::cout << std::endl;

	std::cout << "***************************************************" << std::endl;
	std::cout << "                       RESULTS                     " << std::endl;
	std::cout << "***************************************************" << std::endl;

	std::cout << "Total tests passed: " << _tests_passed << std::endl;
	std::cout << "Total tests failed: " << _tests_failed << std::endl;

	double percentage;

	if (_tests_passed > 0) 
		{ percentage = (_tests_passed/(_tests_passed + _tests_failed)) * 100; }
	else
		{ percentage = 0.0; }

	std::cout << percentage << '%' << " of tests passing." << std::endl;

	std::cout << std::endl;

	// The following statements MUST execute last.
	if(_all_tests_passed)
		std::cout << "Tests passed" << std::endl;
	else
		std::cout << "Tests failed" << std::endl;
	// end of tests.

	
}



// ****************************************************************************
//	Constructor test cases
// ****************************************************************************

// Attempts to create a new connection object with valid ip and port
void test_basic_constructor()
{
	//std::cout << "Attempt to construct with valid ip and port: " << std::endl;

	// Attempt to connect to the spreadsheet server on lab1-12 port 2112.
	connection c = connection("lab1-12.eng.utah.edu", "2112");

	// If c exists and is bound...
	if(c.is_bound())
	{
		_tests_passed++;
	}
		
	else
	{ 
		_all_tests_passed = false;
		_tests_failed++; 
	}
}



std::string int_to_string(const int &val)
{
	std::stringstream ss;
	ss << val;
	std::string str = ss.str();
	return str;
}


// ****************************************************************************
//	safe_queue test cases
// ****************************************************************************

void test_basic()
{
	connection c = connection("lab1-12.eng.utah.edu", "2112");

	if(!c.is_bound())
	{
	      _all_tests_passed = false;
	      _tests_failed++;
	      return;
	}

	c.send_message("connect sysadmin huge");

	// sleep...
	for(int i = 0; i < 10000000; i++) { }

	

	int counter = 0;

	while(true)
	{
		std::string message = "cell A" + int_to_string(counter) +  " foo" + int_to_string(counter);
		c.send_message(message);
		counter++;

		// sleep...
		for(int i = 0; i < 10000000; i++) { }

		std::string result = c.receive_next();

		std::cout << result << std::endl;
	}

	c.close();
}



// ****************************************************************************
//	Edge cases
// ****************************************************************************



// ****************************************************************************
//	Corner cases
// ****************************************************************************



// ****************************************************************************
//	Boundary cases
// ****************************************************************************



// ****************************************************************************
//	Stress cases
// ****************************************************************************

