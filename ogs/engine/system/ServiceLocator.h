#pragma once

extern CDemoAPINull gDemoAPINull;

template<typename T, TDef>
class CServiceLocator
{
public:
	//static void Init(){}
	
	static void Set(T *apNewAPI)
	{
		// Reset to null if invalid or null
		if(!apNewAPI)
		{
			mpCurrentAPI = &mNullAPI;
			return;
		};
		
		mpCurrentAPI = apNewAPI;
	};
	
	static T *Get(){return mpCurrentAPI;}
private:
	static TDef mNullAPI; // TODO: lazy init // problem?
	static T *mpCurrentAPI;
};