#pragma once

#include <windows.h>

#include <memory>
#include <map>
#include <string>

class Context;

class State
{
protected:
	Context& m_context;
	
public:
	State(Context& context);
	virtual ~State() {}
	virtual void update(WPARAM vkCode) = 0;
};

class InitState : public State
{
public:
	InitState(Context& context);
	virtual ~InitState() {}
	virtual void update(WPARAM vkCode);
};

class ErrState : public State
{
public:
	ErrState(Context& context);
	virtual ~ErrState() {}
	virtual void update(WPARAM vkCode);
};

class WorkingState : public State
{
public:
	WorkingState(Context& context);
	virtual ~WorkingState() {}
	virtual void update(WPARAM vkCode);
};

class Context
{
public:
	enum STATES
	{
		INIT,
		ERR,
		WORKING
	};

	Context();
	void setErrorMessage(const std::string& message);
	void setState(Context::STATES state);
	void update(WPARAM vkCode);

protected:
	std::shared_ptr<State> m_currentState;
	std::map<int, std::shared_ptr<State>> m_states;
};
