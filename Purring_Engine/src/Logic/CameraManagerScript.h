#pragma once
#include "Script.h"
#include "Events/EventHandler.h"

namespace PE
{
	struct CameraManagerScriptData
	{
		int NumberOfCamera;
		std::vector<EntityID> CameraIDs;
	};

	class CameraManagerScript : public Script
	{
	public:
		virtual void Init(EntityID id);
		virtual void Update(EntityID id, float deltaTime);
		virtual void Destroy(EntityID id);
		virtual void OnAttach(EntityID id);
		virtual void OnDetach(EntityID id);
		std::map<EntityID, CameraManagerScriptData>& GetScriptData();
		rttr::instance GetScriptData(EntityID id);
		virtual ~CameraManagerScript();
	private:
		void OnKeyTriggered(const Event<KeyEvents>& r_ME);
		void ChangeCamera(EntityID id);
	private:
		std::map<EntityID, CameraManagerScriptData> m_ScriptData;
		bool m_keyPressed;
		int	 m_key;

	};

}