// 
// 
// 

#include "RequestParser.h"

boolean RequestParser::parse(String request)
{
	request.trim();
	if (request.length() <= 0) return false;
	
	int start = request.indexOf("?");
	int end = request.indexOf("=");

	if (start >= 0 && end >= 0 ){
		_key = request.substring(start + 1, end);
		_key.trim();
		if (_key.length() <= 0) return false;
		String val = request.substring(end+1, request.length());
		int http = val.indexOf("HTTP");
		if ( http != -1){
			_value = val.substring(0, http);
			
		}
		_value.trim();
		if (_value.length() <= 0) return false;
		return true;
	}	
	return false;
}
String RequestParser::getKey()
{
	return _key;
}

String RequestParser::getValue()
{
	return _value;
}


