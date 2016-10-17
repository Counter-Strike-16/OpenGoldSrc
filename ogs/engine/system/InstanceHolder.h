#pragma once

template<typename T>
class CInstanceHolder
{
public:
	//static void Init(){}
	
	static void put(T *apNew){mpCurrent = apNew;}
	static T *holds(){return mpCurrent;}
private:
	static T *mpCurrent;
};