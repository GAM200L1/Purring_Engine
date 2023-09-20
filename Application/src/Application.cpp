#include "Purring_Engine.h"

class Application : public PE::CoreApplication
{
public:
	Application()
	{

	}

	~Application()
	{

	}

};

PE::CoreApplication* PE::CreateApplication()
{
	return new Application();
}
