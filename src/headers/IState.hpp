#pragma once
class IState {
private:
	bool m_closeApp;
public:
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
};