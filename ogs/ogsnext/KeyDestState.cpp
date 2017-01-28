class IKeyDestState
{
public:
	virtual ~IKeyDestState()
	{
	}
	virtual void HandleKey(int anKey) = 0;
};

class CKeyDestState_Menu : public IKeyDestState
{
public:
	~CKeyDestState_Menu();

	void HandleKey(int anKey);
};

class CKeyDestState_Message : public IKeyDestState
{
public:
	~CKeyDestState_Message();

	void HandleKey(int anKey);
};

class CKeyDestState_Game : public IKeyDestState
{
public:
	~CKeyDestState_Game();

	void HandleKey(int anKey);
};