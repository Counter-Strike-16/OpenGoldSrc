#pragma once

class Server : public IServer
{
public:
	// WARNING: order is broken!

	void Init();
	void Shutdown();
	
	void RunFrame();
	
	void SetState();
	
	void Connect();
	void SendConnectPacket();
	
	void AcceptChallenge();
	void AcceptBadPassword();
	void AcceptRejection();
	void AcceptConnection();
	void AcceptRedirect();
	
	void ParseVoiceInit();
	void ParseBad();
	void ParseAddAngle();
	void ParseResourceRequest();
	
	void ProcessMessage();
	void ProcessConnectionlessMessage();
	void ProcessConnectionlessServerMessage();
	
	void ExecuteCommand();
};