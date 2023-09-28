/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     Event.h
 \creation date:       30-08-2023
 \last updated:        16-09-2023
 \author:              Jarran TAN Yan Zhi

 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief    This header file declares and defines the template-based Event and EventDispatcher
           classes for event management within the Purring Engine.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include <string>			// For names
#include <map>				// To map functions to event typer
#include <functional>		// To bind functions

namespace PE {

	/*!***********************************************************************************
	 \brief				Base event class for the other events to inherhit
	 \tparam T          Type of Event
	*************************************************************************************/
	template <typename T>
	class Event
	{
		// ----- Public Constructors ----- // 
	public:
		/*!***********************************************************************************
		 \brief     default constructor for events
		*************************************************************************************/
		Event() = default;
		/*!***********************************************************************************
		\brief			Implicit conversion to a std::string, used for output streams.
		\param [in]		T type type of the event
		\param [in]		std::string name of the event.
		*************************************************************************************/
		Event(T type, const std::string& name = "") : m_Type(type), m_Name(name) {}
		virtual ~Event() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Implicit conversion to a std::string, used for output streams.
		 \return    std::string - Returns the name of the event.
		*************************************************************************************/
		virtual std::string ToString() const { return GetName(); }

		/*!***********************************************************************************
		 \brief     Checks if the event has been handled.
		 \return    bool - Returns true if the event has been handled, false otherwise.
		*************************************************************************************/
		virtual bool Handled() const { return m_Handled; }

		// ----- Public getters ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Gets the event type of the current event.
		 \return    T - Returns the event type, which is typically an enum.
		*************************************************************************************/
		inline const T GetType() const { return m_Type; }

		/*!***********************************************************************************
		 \brief     Gets the name of the event.
		 \return    const std::string& - Returns the name of the event.
		*************************************************************************************/
		inline const std::string& GetName() const { return m_Name; }

		// ----- Protected variables ----- // 
	protected:
		T m_Type;
		std::string m_Name;
		bool m_Handled = false;
	};

	/*!***********************************************************************************
	 \class     EventDispatcher<T>
	 \brief     A templated class responsible for dispatching events to registered listeners.
	 \details   Supports the addition of listeners and the broadcasting of events.
	*************************************************************************************/
	template<typename T>
	class EventDispatcher
	{
		// ----- Definition ----- // 
		using Func = std::function<void(const Event<T>&)>;
		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     Adds a listener for a specific event type.
		 \param[in] T type - The type of event to listen to.
		 \param[in] const Func& func - The callback function to be invoked when the event is dispatched.
		*************************************************************************************/
		void AddListener(T type, const Func& func) { m_Listerners[type].push_back(func); }				// Map type to a function


		// To be defined within engine to makesure only accessible through the engine
		/*!***********************************************************************************
		 \brief     Dispatches an event to all registered listeners of its type.
		 \param[in] Event<T>& event - The event object to be dispatched.
		*************************************************************************************/
		void SendEvent(const Event<T>& event)
		{
			//check if there is a listerner for this event
			if (m_Listerners.find(event.GetType()) == m_Listerners.end())
				return;

			//loop through all listerners if the event is not handled we process it.
			for (auto&& listener : m_Listerners.at(event.GetType()))
			{
				if (!event.Handled()) listener(event);
			}
		}
		// ----- Private variables ----- // 
	private:
		std::map<T, std::vector<Func>> m_Listerners;
	};

	/*!***********************************************************************************
	 \brief     Overloads the output stream operator for easy logging of Event objects.
	 \param[out] std::ostream& os - The output stream.
	 \param[in]  Event<T>& e - The event to log.
	 \return    std::ostream& - Returns the modified output stream.
	*************************************************************************************/
	template<typename T>
	inline std::ostream& operator<<(std::ostream& os, const Event<T>& e)
	{
		return os << e.ToString() << "\n";
	}

} // namespace temp

