class CInputHandler
{
public:
	void Update();

	void HandleKey(int anKey);

private:
	std::vector<IKeyDestState *> mvKeyDestStates;
	IKeyDestState meKeyDestState;
};

void CInputHandler::Update(){};

void CInputHandler::HandleKey(int anKey)
{
	mvKeyDestStates[meKeyDestState]->HandleKey(anKey);
};

void CInputHandler::SetKeyDestState(IKeyDestState aeState)
{
	if(meKeyDestState != aeState)
		meKeyDestState = aeState;
};

IKeyDestState CInputHandler::GetKeyDestState()
{
	return meKeyDestState;
};

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