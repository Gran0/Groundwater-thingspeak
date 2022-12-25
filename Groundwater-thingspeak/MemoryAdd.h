#pragma once
class MemoryAdd
{
public:
	//               Key     Address   Byte size
	static const int WiFiName		= 0;   //40b
	static const int WiFipasswd		= 40;  //40b

	static const int APname			= 80;  // 30b
	static const int APpasswd		= 110; // 40b
	static const int APchannel		= 150; // 1b
	static const int SensorHeight	= 151; // 2b

	static const int INICIALIZED	= 155; // 1b
	static const int ApiKey			= 156; // 20b
	static const int PeriodicalMode = 177; // 1b
	static const int Period			= 178; // 2b
	static const int FieldID		= 180; // 2b
	static const int LocalServerIp	= 182; // 4b
	static const int UseLocalServ	= 186; // 1b
	static const int LocalServerPort= 187; // 2b
private:
};

