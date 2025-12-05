#pragma once

#include <vector>
#include <memory>
#include "IState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

class StateManager {
    public:
	        
        // Default constructor
        StateManager() = default;
	        
        // Destructor
        ~StateManager();        
            
        // Replace entire stack with nextState (deferred)
        void ChangeState(std::unique_ptr<IState> nextState);
            
        // Push (deferred)
        void PushState(std::unique_ptr<IState> nextState);
            
        // Pop (deferred)
        void PopState();
            
        // Called every frame from main loop
        void UpdateState();
            
        // Render all states (bottom -> top)
        void RenderState(sf::RenderWindow& window);
            
    private:
		
        // Pending operation structure
		enum class PendingType {NONE = 0, CHANGE, PUSH, POP};
		
        // Pending operation data
        struct Pending {
            PendingType m_type = PendingType::NONE;
            std::unique_ptr<IState> m_states;};
		
        // Stack of states
		std::vector<std::unique_ptr<IState>> m_states;
            
        // Penging object
		Pending m_pending;
		
        // Process pending operations
        void ProcessPending();
};
