#pragma once
typedef unsigned long long EntityID;

namespace PE
{
    enum class CSharpScriptState { INIT, UPDATE, EXIT };

    class CSharpScript
    {
    public:
        virtual void Init(EntityID) = 0;
        virtual void Update(EntityID, float) = 0;
        virtual void Destroy(EntityID) = 0;
        virtual void OnAttach(EntityID) = 0;
        virtual void OnDetach(EntityID) = 0;

        virtual ~CSharpScript() {}
    };
}
