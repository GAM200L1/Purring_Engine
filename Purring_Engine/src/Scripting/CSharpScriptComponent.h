#pragma once
typedef unsigned long long EntityID; ///< Type definition for a unique identifier for entities.

namespace PE
{
    /*!***********************************************************************************
     \brief     Represents the possible states of a C# script within the game engine.
    *************************************************************************************/
    enum class CSharpScriptState { INIT, UPDATE, EXIT };

    /*!***********************************************************************************
     \brief     Abstract base class for C# scripting integration, defining lifecycle hooks.
    *************************************************************************************/
    class CSharpScript
    {
    public:
        /*!***********************************************************************************
         \brief     Initializes the script for the given entity.
         \param     EntityID The unique identifier for the entity.
        *************************************************************************************/
        virtual void Init(EntityID) = 0;

        /*!***********************************************************************************
         \brief     Updates the script for the given entity with the time step.
         \param     EntityID The unique identifier for the entity.
         \param     float The time step for the update.
        *************************************************************************************/
        virtual void Update(EntityID, float) = 0;

        /*!***********************************************************************************
         \brief     Cleans up the script before the entity is destroyed.
         \param     EntityID The unique identifier for the entity.
        *************************************************************************************/
        virtual void Destroy(EntityID) = 0;

        /*!***********************************************************************************
         \brief     Called when the script is attached to an entity.
         \param     EntityID The unique identifier for the entity.
        *************************************************************************************/
        virtual void OnAttach(EntityID) = 0;

        /*!***********************************************************************************
         \brief     Called when the script is detached from an entity.
         \param     EntityID The unique identifier for the entity.
        *************************************************************************************/
        virtual void OnDetach(EntityID) = 0;

        /*!***********************************************************************************
         \brief     Virtual destructor for safe polymorphic destruction.
        *************************************************************************************/
        virtual ~CSharpScript() {}
    };
}
