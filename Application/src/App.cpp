#include "Purring_Engine.h"

class Test : public Purr::Application
{
public:
	Test()
	{

	}

	~Test()
	{

	}

};

Purr::Application* Purr::CreateApplication()
{
	return new Test();
}