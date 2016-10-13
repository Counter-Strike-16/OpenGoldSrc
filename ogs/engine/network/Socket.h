#pragma once

class qsocket_t // CNetChannel
{
public:
	// Returns true or false if the given qsocket can currently accept a
	// message to be transmitted
	bool CanSendMessage();
	
	// Returns data in net_message sizebuf
	// returns 0 if no data is waiting
	// returns 1 if a message was received
	// returns 2 if an unreliable message was received
	// returns -1 if the connection died
	int GetMessage();
	
	// returns 0 if the message connot be delivered reliably,
	// but the connection is still considered valid
	//
	// returns 1 if the message was sent properly
	// returns -1 if the connection died
	int SendMessage(sizebuf_t *data);
	int SendUnreliableMessage(sizebuf_t *data);
	
	qsocket_t *next;
	
	double connecttime;
	double lastMessageTime;
	double lastSendTime;
	
	bool mbDisconnected;
	bool canSend;
	bool sendNext;
	
	int driver;
	int landriver;
	int socket;
	
	void *driverdata;

	uint ackSequence;
	uint sendSequence;
	uint unreliableSendSequence;
	
	int sendMessageLength;
	byte sendMessage[NET_MAXMESSAGE];
	
	uint receiveSequence;
	uint unreliableReceiveSequence;
	
	int receiveMessageLength;
	byte receiveMessage[NET_MAXMESSAGE];
	
	qsockaddr addr;
	char address[NET_NAMELEN];
};