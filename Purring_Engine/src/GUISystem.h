#include "System.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"

namespace PE {
	class GUISystem : public System
	{
	public:
		/*!***********************************************************************************
		 \brief     Virtual destructor for proper cleanup of derived systems.
		*************************************************************************************/
		virtual ~GUISystem();

		/*!***********************************************************************************
		 \brief     Initialize the system. Should be called once before any updates.
		*************************************************************************************/
		virtual void InitializeSystem() override;

		/*!***********************************************************************************
		 \brief     Update the system each frame.
		 \param     deltaTime Time passed since the last frame, in seconds.
		*************************************************************************************/
		virtual void UpdateSystem(float deltaTime) override;

		/*!***********************************************************************************
		 \brief     Clean up the system resources. Should be called once after all updates.
		*************************************************************************************/
		virtual void DestroySystem() override;

		/*!***********************************************************************************
		 \brief     Get the system's name, useful for debugging and identification.
		 \return    std::string The name of the system.
		*************************************************************************************/
		virtual std::string GetName() override;

		void OnMouseClick(const Event<MouseEvents>& r_ME);

		void OnHover();
	};

	struct GUI
	{
		//enum to tell type of UI to make
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Destroy() = 0;
		virtual void OnHover() = 0;
		virtual void OnClick() = 0;
		virtual ~GUI() {};
	};


	struct Button : public GUI
	{
		virtual void Init() override;
		virtual void Update() override;
		virtual void Destroy() override;
		//use this for now idk how are we gonna bind functions later
		virtual void OnHover() override;
		virtual void OnClick() override;
		virtual ~Button() {};

		float clicked;//check this for click
		float bufferTime;//the amt to add to click 
		std::function<void (*)(void)> onClicked;
		std::function<void (*)(void)> onHovered;
	};
}