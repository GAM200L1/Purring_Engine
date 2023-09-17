/*!***********************************************************************************

 \project  Purring Engine
 \module   CSD2401-A
 \file     Event.h
 \date     8/30/2023

 \author               Jarran Tan Yan Zhi
 \par      email:      jarranyanzhi.tan@digipen.edu

 \brief
	Header file containing the declaration and definition of the event and event dispatcher template

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.

*************************************************************************************/

#ifndef EVENT_H
#define EVENT_H

#include <string> // for names
#include <map> // to map functions to event typer
#include <functional> // to bind functions

namespace temp {

	/*!***********************************************************************************
	 \brief				Base event class for the other events to inherhit
	 \tparam T          Type of Event
	*************************************************************************************/
	template <typename T>
	class Event
	{
		// ----- Protected variables ----- // 
	protected:
		T m_Type;
		std::string m_Name;
		bool m_Handled = false;

		// ----- Constructors ----- // 
	public:
		Event() = default;
		Event(T type, const std::string& name = "") : m_Type(type), m_Name(name) {}
		virtual ~Event() {}

		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************
		 \brief     implicit conversion to a std::string, used for output streams

		 \return std::string        by default returns the name of the event
		*************************************************************************************/
		virtual std::string ToString() const { return GetName(); }
		/*!***********************************************************************************

		\brief			Has the event been handled

		\return bool	returns whether the event has been handled

		*************************************************************************************/
		virtual bool Handled() const
		{
			return m_Handled;
		}

		// ----- Public getters ----- // 
	public:
		/*!***********************************************************************************

		 \brief				Returns the event type of the current event.

		 \return T			Returns the event type, which is an enum

		*************************************************************************************/
		inline const T GetType() const {
			return m_Type;
		}

		/*!***********************************************************************************

		 \brief							return the name of the event

		 \return const std::string		the name of the current event

		*************************************************************************************/
		inline const std::string& GetName() const
		{
			return m_Name;
		}
	};

	template<typename T>
	class EventDispatcher
	{
		// ----- Private variables ----- // 
	private:
		using Func = std::function<void(const Event<T>&)>;
		std::map<T, std::vector<Func>> m_Listerners;
		// ----- Public methods ----- // 
	public:
		/*!***********************************************************************************

		 \brief		Subscrive an object as a listerner of a specific event



		 \param[in] T type The type of event to subscribe to

		 \param[in] const Func& A function that returns void and takes in an event

		*************************************************************************************/
		void AddListener(T type, const Func& func)
		{
			//map type to a function
			m_Listerners[type].push_back(func);
		}

		//to be defined within engine to makesure only accessible through the engine
		/*!***********************************************************************************

		 \brief		Send an event of a specific type, then call out to all the listeners subscribed to the event

		 \param[in] Event<T>& event Any event

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
	};

	/*!***********************************************************************************

	 \brief		ostream overload to print out the event as a string

	 \param[out] std::ostream

	*************************************************************************************/
	template<typename T>
	inline std::ostream& operator<<(std::ostream& os, const Event<T>& e)
	{
		return os << e.ToString() << "\n";
	}













}

#endif