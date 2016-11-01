#pragma once

class CNetBuffer // CNetMsg/bf_readwrite
{
public:
	CNetBuffer();
	~CNetBuffer();
	
	void Clear();
	
	void *GetSpace(int length);
	
	void Write(void *data, int length);
	
	// strcats onto the sizebuf
	void Print(char *data);
private:
	byte *data;
	
	int maxsize;
	int cursize;
	
	bool allowoverflow; // if false, do a Sys_Error
	bool overflowed; // set to true if the buffer size failed
};