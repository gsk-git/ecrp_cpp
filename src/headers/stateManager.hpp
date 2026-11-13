#pragma once

#include "gameState.hpp"
#include "splash.hpp"
#include "pauseState.hpp"
#include <memory>
#include <vector>

class stateManager {
private:
	std::vector<std::unique_ptr<IState>> m_states;
public:
	sf::RenderWindow& initWindowHandle;

	stateManager(sf::RenderWindow& window) : initWindowHandle(window) {
		pushState(std::make_unique<splash> (initWindowHandle));
	};

	void pushState(std::unique_ptr<IState> state) {
		m_states.push_back(std::move(state));
		m_states.back()->init();
		m_states.back()->onEnter();
		std::cout << "States Count" << m_states.size();
	};
	
	void popState() {
		if (m_states.empty()) return
		m_states.back()->onExit();
		m_states.pop_back();
		if (!m_states.empty()) m_states.back()->onEnter();
	};

	void replaceState(std::unique_ptr<IState> state)
	{
		if (!m_states.empty())
		{
			m_states.back()->onExit();
			m_states.pop_back();
		}

		m_states.push_back(std::move(state));

		m_states.back()->init();
		m_states.back()->onEnter();
	}
	
	void handleEvent(const sf::Event& event) {
		if (!m_states.empty()) m_states.back()->handleEvent(event);
	};
	
	void update(float& dt) {
		if (!m_states.empty()) m_states.back()->update(dt);
	};
	
	void render(sf::RenderWindow& window) {
		if (!m_states.empty()) m_states.back()->render(window);
	};
	
	bool isAppCloseRequested() {
		return !m_states.empty() && m_states.back()->shouldCloseApp();
	};
	
	IState* getCurrentState() {
		return m_states.empty() ? nullptr : m_states.back().get();
	};
	
	bool processPendingRequest() {
		if (!m_states.empty() && m_states.back()->shouldCloseApp()) return true;
		if (!m_states.empty() && m_states.back()->shouldReplaceGame()) replaceState(std::make_unique<gameState>(initWindowHandle));
		if (!m_states.empty() && m_states.back()->shouldOpenPause()) pushState(std::make_unique<pauseState>(initWindowHandle));
		if (!m_states.empty() && m_states.back()->shouldClosePause()) popState();
		return false;
	}
};
