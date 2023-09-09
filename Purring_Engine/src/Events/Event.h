#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <map>
#include <functional>
#include <vector>

namespace temp {

	//base class for events to inherit	
	template <typename T>
	class Event
	{
	protected:
		T m_Type;
		std::string m_Name;
		bool m_Handled = false;
	public:
		Event() = default;
		Event(T type, const std::string& name = "") : m_Type(type), m_Name(name) {}
		virtual std::string ToString() const { return GetName(); }
		virtual ~Event() {}

		inline const T GetType() const { 
			return m_Type; 
		}

		template<typename EventType>
		inline EventType ToType() const
		{
			return static_cast<const EventType&>(*this);
		}

		inline const std::string& GetName() const 
		{ 
			return m_Name; 
		}

		virtual bool Handled() const 
		{
			return m_Handled; 
		}

	};


	//dispatcher
	template<typename T>
	class EventDispatcher 
	{
	private:
		using Func = std::function<void(const Event<T>&)>;
		std::map<T, std::vector<Func>> m_Listerners;
	public:
		void AddListener(T type, const Func& func)
		{
			m_Listerners[type].push_back(func);
		}

		//to be defined within engine to makesure only accessible through the engine

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

	template<typename T>
	inline std::ostream& operator<<(std::ostream& os, const Event<T>& e)
	{
		return os << e.ToString() << "\n";
	}













}

#endif