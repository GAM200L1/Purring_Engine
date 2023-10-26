#pragma once
typedef unsigned long long EntityID;

namespace PE
{
	class StateMachine;

	class State
	{
	protected:
		StateMachine* p_stateMachine;
	public:
		void SetStateMachine(StateMachine* sm) { p_stateMachine = sm; }
		virtual ~State() {}
		virtual void StateEnter(EntityID id) = 0;
		virtual void StateUpdate(EntityID id, float deltaTime) = 0;
		virtual void StateExit(EntityID id) = 0;
		virtual std::string_view GetName() = 0;
	};

	class StateMachine
	{
	protected:
		State* p_state;
	public:
		StateMachine() : p_state(nullptr) {}
		~StateMachine() { delete p_state; }

		virtual void ChangeState(State* state, EntityID id)
		{
			if (p_state)
			{
				DoStateExit(id);
				delete p_state;
			}
			p_state = state;
			p_state->SetStateMachine(this);
			DoStateEnter(id);
		}

		virtual void Update(EntityID id, float deltaTime)
		{
			if(p_state)
			DoStateUpdate(id, deltaTime);
		}

		std::string_view GetStateName() { return p_state->GetName(); }
	private:
		virtual void DoStateEnter(EntityID id) { p_state->StateEnter(id); }
		virtual void DoStateUpdate(EntityID id, float deltaTime) { p_state->StateUpdate(id, deltaTime); }
		virtual void DoStateExit(EntityID id) { p_state->StateExit(id); }
	};



}