#pragma once
class MemoryAdd
{
public:
	//               Key     Address   Byte size
	static const int WiFiName   = 0;   //40b
	static const int WiFipasswd = 40;  //40b

	static const int APname     = 80;  // 30b
	static const int APpasswd   = 110; // 40b
	static const int APchannel  = 150; // 4b

	static const int INICIALIZED = 155; // 1b
	static const int ApiKey      = 156; // 20b

	


private:
};

