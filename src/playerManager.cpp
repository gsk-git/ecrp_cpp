// Including necessary libraries
#include "headers/playerManager.hpp"
#include <array>
#include "SFML/Graphics/Texture.hpp"
#include <string>
#include <iostream>
#include <utility>
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"

float speed = 100.0f;
float movedirx = 0.0f;
float movediry = 0.0f;
float boost = 0.0f;
float dt = 0.0f;
float totalspeed = 0.0f;
int jumpboost = 0;
bool Save = false;
std::string playerFileURI = "res\\user_dat\\";
std::string saveFile = "data.json";
int frame_count = 0;
std::pair<float, float> PLAYER_POSITION = { 0.0f, 0.0f };
std::array<sf::Texture, StateCount> kTextures;

Player::Player() {
	// Initializing member variables
	m_IsMoving = false;
	m_IsSlashAttacking = false;
	m_IsJumping = false;
	m_IsSitting = false;
	m_IsRunning = false;
	m_IsTileOcean = false;
	m_StateEnum = State::idle;
	m_PrevStateEnum = State::idle;
	m_DirectionEnum = Directions::down;
	m_playerXY = sf::Vector2f(0.0f, 0.0f);
	m_TotalFrames = 0;
	m_CurrentFrame = 0;
	m_AnimTimer = 0.0f;
	m_AnimDuration = 0.0f;
	m_health = 500u;
	m_playersprite.emplace(kTextures[to_index(m_StateEnum)]);
	m_playersprite->setTextureRect(sf::IntRect({ m_CurrentFrame * PLAYER_SPRITE, static_cast<int>(to_index(m_DirectionEnum)) * PLAYER_SPRITE }, { PLAYER_SPRITE, PLAYER_SPRITE }));
	m_playersprite->setPosition({ 0, 0 });
	m_playersprite->setScale({ 1.f, 1.f });
	setOrigintoBottomCenter();
}

void Player::setOrigintoBottomCenter() {
	// Setting origin to bottom center of player sprite
	if (m_playersprite) {
		sf::FloatRect bounds = m_playersprite->getLocalBounds();
		m_playersprite->setOrigin({ bounds.size.x / 2.0f, bounds.size.y - 1 });
	}
}

void Player::update(float dt) {
	// Rendering and animating player sprite
	animatesprite(dt);
}

void Player::animatesprite(float dt) {

	// Flag check for player movement
	m_IsMoving = (movedirx != 0 || movediry != 0);

	// Setting animation duration based on player state
	m_AnimDuration = (m_IsMoving || m_IsJumping) ? 0.1f : 0.3f;

	// Setting player state, animation duration and handling other EDGE cases
	if (m_IsJumping) {
		m_StateEnum = State::jump;
		m_IsSitting = false;
	}
	else if (m_IsMoving) {
		m_StateEnum = State::walk;
		m_IsSitting = false;
		if (m_IsRunning) {
			m_StateEnum = State::run;
		}
		if (m_IsJumping) {
			m_StateEnum = State::jump;
		}
	}
	else if (m_IsSitting) {
		m_StateEnum = State::sit;
	}
	else if (m_IsSlashAttacking) {
		m_StateEnum = State::slash;
		m_IsSitting = false;
	}
	else {
		m_StateEnum = State::idle;
		m_IsSitting = false;
	}

	// Resetting animation if there is change in player's state
	if (m_StateEnum != m_PrevStateEnum) {
		m_playersprite->setTexture(kTextures[to_index(m_StateEnum)]);
		m_PrevStateEnum = m_StateEnum;
		m_CurrentFrame = 0;
		m_AnimTimer = 0.0f;
	}

	// Updating animation timing @ every frames
	m_AnimTimer += dt;

	// Checking player state
	if (m_StateEnum != State::sit && m_StateEnum != State::jump) {
		while (m_AnimTimer >= m_AnimDuration) {
			m_AnimTimer -= m_AnimDuration;
			m_CurrentFrame++;
			if (m_CurrentFrame >= kFrameCount[to_index(m_StateEnum)])
				m_CurrentFrame = 0;
		}
	}
	else if (m_StateEnum == State::sit) {
		m_CurrentFrame = 0;
	}
	else if (m_StateEnum == State::jump) {
		while (m_AnimTimer >= m_AnimDuration) {
			m_AnimTimer -= m_AnimDuration;
			m_CurrentFrame++;
			if (m_CurrentFrame == 2)
				jumpboost = 150;
			if (m_CurrentFrame >= kFrameCount[to_index(m_StateEnum)]) {
				m_CurrentFrame = 0;
				m_IsJumping = false;
				jumpboost = 0;
			}
		}
	}

	// Updating texture rectangle
	m_playersprite->setTextureRect(sf::IntRect({ m_CurrentFrame * PLAYER_SPRITE, static_cast<int>(to_index(m_DirectionEnum)) * PLAYER_SPRITE }, { PLAYER_SPRITE, PLAYER_SPRITE }));
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Draws player object
	states.transform *= getTransform();
	target.draw(*m_playersprite, states);
}