#pragma once
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
		virtual void StateEnter() = 0;
		virtual void StateUpdate() = 0;
		virtual void StateExit() = 0;
		virtual std::string_view GetName() = 0;
	};

	class StateMachine
	{
	private:
		State* p_state;
	public:
		StateMachine() : p_state(nullptr) {}
		~StateMachine() { delete p_state; }

		void ChangeState(State* state)
		{
			if (p_state)
			{
				DoStateExit();
				delete p_state;
			}
			p_state = state;
			p_state->SetStateMachine(this);
			DoStateEnter();
		}

		void Update()
		{
			if(p_state)
			DoStateUpdate();
		}

		std::string_view GetStateName() { return p_state->GetName(); }
	private:
		void DoStateEnter() { p_state->StateEnter(); }
		void DoStateUpdate() { p_state->StateUpdate(); }
		void DoStateExit() { p_state->StateExit(); }
	};



}