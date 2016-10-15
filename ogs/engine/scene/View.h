#pragma once

class CView
{
public:
	CView();
	~CView();
	
	void StartPitchDrift();
	void StopPitchDrift();
	
	void RenderView();
	void UpdatePalette();
	void Register();
	void ParseDamage();
	void SetContentsColor(int contents);
	void CalcBlend();
};