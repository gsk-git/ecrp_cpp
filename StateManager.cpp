#include "StateManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <utility>
#include "IState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"
#include "SplashState.hpp"

StateManager::~StateManager() {
	while (!m_states.empty()) {
		m_states.back()->OnExit();
		m_states.pop_back();
	}
}

void StateManager::ChangeState(std::unique_ptr<IState> nextState) {
	m_pending.m_states = std::move(nextState);
	m_pending.m_type = PendingType::CHANGE;
}

void StateManager::PushState(std::unique_ptr<IState> nextState) {
	m_pending.m_states = std::move(nextState);
	m_pending.m_type = PendingType::PUSH;
}

void  StateManager::PopState() {
	m_pending.m_type = PendingType::POP;
}

void StateManager::UpdateState() {
	
	if (m_states.empty()) return;
	
	IState* Top = m_states.back().get();
	int raw = Top->Update();
	StateSignal sig = static_cast<StateSignal>(raw);
	
	switch (sig) {
		case StateSignal::NONE:
			break;
		case StateSignal::CHANGE_TO_GAMESTATE:
			m_pending.m_states = std::make_unique<GameState>();
			m_pending.m_type = PendingType::CHANGE;
			break;
		case StateSignal::CHANGE_TO_PAUSESTATE:
			m_pending.m_states = std::make_unique<PauseState>();
			m_pending.m_type = PendingType::PUSH;
			break;
		case StateSignal::EXIT_APPLICATION:
			m_pending.m_states.reset();
			m_pending.m_type = PendingType::CHANGE;
			break;
	}
	
	ProcessPending();
}

void StateManager::ProcessPending() {
	switch (m_pending.m_type) {
		case PendingType::NONE:
				break;
		case PendingType::CHANGE:
			while (!m_states.empty()) {
				m_states.back()->OnExit();
				m_states.pop_back();
			}
			if (m_pending.m_states) {
				m_pending.m_states->OnEnter();
				m_states.push_back(std::move(m_pending.m_states));
			}
			break;
		case PendingType::POP:
			if (!m_states.empty()) {
				m_states.back()->OnEnter();
				m_states.pop_back();
			}
			break;
		case PendingType::PUSH:
			if (m_pending.m_states) {
				m_pending.m_states->OnEnter();
				m_states.push_back(std::move(m_pending.m_states));
			}
			break;
	}
	m_pending.m_type = PendingType::NONE;
	m_pending.m_states.reset();
}

void StateManager::RenderState(sf::RenderWindow& window) {
	for (auto& s : m_states)
		s->Render(window);
}