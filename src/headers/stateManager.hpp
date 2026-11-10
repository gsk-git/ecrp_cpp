#pragma once

#include "gameState.hpp"
#include <memory>
#include <vector>

class stateManager {
private:
	std::vector<std::unique_ptr<IState>> m_states;
public:
	void pushState(std::unique_ptr<IState> state) {
		m_states.push_back(std::move(state));
		m_states.back()->init();
	};
	void popState() {
		if (!m_states.empty()) m_states.pop_back();
	};
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
		return m_states.back()->shouldCloseApp();
	};
	IState* getCurrentState() {
		return m_states.empty() ? nullptr : m_states.back().get();
	};
};
