// RequestParser.h

#ifndef _REQUESTPARSER_h
#define _REQUESTPARSER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RequestParser
{
 protected:
	 String _key;
	 String _value;

 public:	
	boolean parse(String x);
	String getKey();
	String getValue();
};

//extern RequestParser RequestParser;

#endif

