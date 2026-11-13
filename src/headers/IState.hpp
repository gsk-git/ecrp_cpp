#pragma once
#include <iostream>

enum class stateType {
	Splash,
	GameMenu,
	SettingMenu,
	PauseMenu
};

class IState {
private:
	
public:
	bool m_closeApp = false;
	bool m_replaceGame = false;
	bool m_openPause = false;
	bool m_closePause = false;
	sf::RenderWindow& m_windowHandle;
	IState(sf::RenderWindow& window) : m_windowHandle(window) {};
	virtual ~IState() {};
	virtual void init() = 0;
	virtual void handleEvent(const sf::Event& event) = 0;
	virtual void update(float dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	// State policies
	virtual bool updateBelow() = 0;
	virtual bool renderBelow() = 0;
	virtual bool shouldCloseApp() const { return false;}
	virtual bool shouldReplaceGame() const { return false; }
	virtual bool shouldOpenPause() const { return false; }
	virtual bool shouldClosePause() const { return false; }
	virtual stateType getStateType() const = 0;
};