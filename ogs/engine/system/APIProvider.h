#pragma once

template<typename T, TDef>
class CProvider
{
public:
	//static void Init(){}
	
	static void Set(T *apNew)
	{
		// Reset to null if invalid or null
		if(!apNew)
		{
			mpCurrent = &mNull;
			return;
		};
		
		mpCurrent = apNew;
	};
	
	static T *Get(){return mpCurrent;}
private:
	static TDef mNull; // TODO: lazy init // problem?
	static T *mpCurrent;
};