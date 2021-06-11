#include "State.h"

#include "metaData.h"
#include "Console.h"
#include "Memory.h"

extern Console					g_console;
extern std::unique_ptr<Memory>	g_memory;

State::State(Context& context)
	: m_context(context)
{
}

InitState::InitState(Context& context)
	: State(context)
{
}

void InitState::update(WPARAM vkCode)
{
}

ErrState::ErrState(Context& context)
	: State(context)
{
}

void ErrState::update(WPARAM vkCode)
{
	if (VK_ESCAPE == vkCode)
	{
		Exit(0);
	}
}

WorkingState::WorkingState(Context& context)
	: State(context)
{
}

void WorkingState::update(WPARAM vkCode)
{
	g_memory->update(vkCode);
}

Context::Context()
	: m_currentState(nullptr)
{
	m_states[Context::STATES::INIT]		= std::make_shared<InitState>(*this);
	m_states[Context::STATES::ERR]		= std::make_shared<ErrState>(*this);
	m_states[Context::STATES::WORKING]	= std::make_shared<WorkingState>(*this);

	this->setState(Context::STATES::INIT);
}

void Context::setErrorMessage(const std::string& message)
{
	g_console.append("Press ESC to exit");
	g_console.append("");
	g_console.append(message);
}

void Context::setState(Context::STATES state)
{
	m_currentState = m_states[state];
}

void Context::update(WPARAM vkCode)
{
	m_currentState->update(vkCode);
}
