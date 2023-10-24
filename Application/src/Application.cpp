#include "Purring_Engine.h"
#include "Scripts/EnemyTestScript.h"
//#include "Scripts/testScript.h"
//#include "Scripts/testScript2.h"
//#include "Scripts/PlayerControllerScript.h"
#include "Logic/LogicSystem.h"

class Application : public PE::CoreApplication
{
public:
	Application()
	{
		//REGISTER_SCRIPT(testScript);
		//REGISTER_SCRIPT(testScript2);
		//REGISTER_SCRIPT(PlayerControllerScript);
		REGISTER_SCRIPT(EnemyTestScript);
	}

	~Application()
	{

	}

};

PE::CoreApplication* PE::CreateApplication()
{
	return new Application();
}
