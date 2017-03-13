#pragma once

struct IMouse
{
	virtual void SetActive(bool abActive) = 0;
	virtual bool IsActive() = 0;
	
	//virtual void MouseEvent(int mstate) = 0;
};