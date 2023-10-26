#pragma once
typedef unsigned long long EntityID;

namespace PE
{
	class StateMachine;

	class State
	{
	public:
		/*!***********************************************************************************
		 \brief						Function to set the current statemachine that is running this state, so that we can access the state machine's function if needed
		 \param [In] StateMachine*	pointer to the state machine running this state
		*************************************************************************************/
		inline void SetStateMachine(StateMachine* sm) { p_stateMachine = sm; }
		/*!***********************************************************************************
		 \brief					Destructor for the State
		*************************************************************************************/
		virtual ~State() {}
		/*!***********************************************************************************
		 \brief					The function to run on enter of the state
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void StateEnter(EntityID id) = 0;
		/*!***********************************************************************************
		 \brief					The function that is ran by the update in the state machine
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void StateUpdate(EntityID id, float deltaTime) = 0;
		/*!***********************************************************************************
		 \brief					The function to run when state is changed away
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		virtual void StateExit(EntityID id) = 0;
		/*!***********************************************************************************
		 \brief					To get the name of the state if we need it
		 \return				The name of the current state
		*************************************************************************************/
		virtual std::string_view GetName() = 0;
	protected:
		StateMachine* p_stateMachine;
	};

	class StateMachine
	{
	public:
		/*!***********************************************************************************
		 \brief					Constructor for the State Machine
		*************************************************************************************/
		StateMachine() : p_state(nullptr) {}
		/*!***********************************************************************************
		 \brief					Destructor for the State Machine
		*************************************************************************************/
		~StateMachine() { delete p_state; }
		/*!***********************************************************************************
		 \brief					The function to change state and call exit and enter state
		 \param [In] State*		The state to change to
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		inline virtual void ChangeState(State* state, EntityID id)
		{
			//if there is a previous state
			if (p_state)
			{
				//Exit previous state
				DoStateExit(id);
				delete p_state;
			}
			//set new state
			p_state = state;
			p_state->SetStateMachine(this);
			//Enter new state
			DoStateEnter(id);
		}
		/*!***********************************************************************************
		 \brief					The function that calls state update
		 \param [In] EntityID	The ID of the object currently running the script
		 \param [In] deltaTime	The DeltaTime of the system
		*************************************************************************************/
		inline virtual void Update(EntityID id, float deltaTime)
		{
			//if state exist update
			if(p_state)
			DoStateUpdate(id, deltaTime);
		}
		/*!***********************************************************************************
		 \brief						to get the name of the state for use
		 \return std::string_view	the name of the state
		*************************************************************************************/
		inline std::string_view GetStateName() { return p_state->GetName(); }
	protected:
		State* p_state;
	private:
		/*!***********************************************************************************
		 \brief					Run the state's enter function
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		inline virtual void DoStateEnter(EntityID id) { p_state->StateEnter(id); }
		/*!***********************************************************************************
		 \brief					Run the state update function
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		inline virtual void DoStateUpdate(EntityID id, float deltaTime) { p_state->StateUpdate(id, deltaTime); }
		/*!***********************************************************************************
		 \brief					Run the state's Exit function
		 \param [In] EntityID	The ID of the object currently running the script
		*************************************************************************************/
		inline virtual void DoStateExit(EntityID id) { p_state->StateExit(id); }
	};



}