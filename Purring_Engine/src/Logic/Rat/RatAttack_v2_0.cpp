#include "prpch.h"
#include "RatScript_v2_0.h"
#include "Logic/Rat/RatAttack_v2_0.h"
#include "Events/EventHandler.h"
#include "Logic/CatScript.h"
#include "Logic/FollowScript.h"

namespace PE
{
    // Constructor
    RatAttack_v2_0::RatAttack_v2_0() : gameStateController(nullptr), p_data(nullptr) {}


    void RatAttack_v2_0::StateEnter(EntityID id)
    {
        p_data = GETSCRIPTDATA(RatScript_v2_0, id);
        gameStateController = GETSCRIPTINSTANCEPOINTER(GameStateController_v2_0);                                                           // Get GSM instance

        m_attackEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerEnter, RatAttack_v2_0::OnTriggerEnterForAttack, this);
        m_attackStayEventListener = ADD_COLLISION_EVENT_LISTENER(CollisionEvents::OnTriggerStay, RatAttack_v2_0::OnTriggerStayForAttack, this);

        m_delay = p_data->attackDelay;
        std::cout << "RatAttack_v2_0::StateEnter - Rat ID: " << id << " has successfully entered the Attack state.\n";

        std::cout << "RatAttack_v2_0::StateEnter - Rat ID: " << id << " has entered the attack state." << std::endl;
    }

    void RatAttack_v2_0::StateUpdate(EntityID id, float deltaTime)
    {
        std::cout << "RatAttack_v2_0::StateUpdate - Rat ID: " << id << " is updating." << std::endl;

        if (gameStateController->currentState == GameStates_v2_0::PAUSE)
        {
            std::cout << "RatMovement_v2_0::StateUpdate - Game is paused." << std::endl;
            return;
        }

        if (m_delay > 0.f && p_data->attacking)
        {
            m_delay -= deltaTime;
        }
        else if (p_data->attacking)
        {
            RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, true);
            //if (EntityManager::GetInstance().Get<AnimationComponent>(id).GetCurrentFrameIndex() == EntityManager::GetInstance().Get<AnimationComponent>(id).GetAnimationMaxIndex())
            //{
            //    RatScript_v2_0::ToggleEntity(p_data->attackTelegraphEntityID, false);
            //    p_data->finishedExecution = true;
            //}

            if (!attacksoundonce)
            {
                SerializationManager serializationManager;
                EntityID sound = serializationManager.LoadFromFile("AudioObject/Rat Attack SFX_Prefab.json");
                if (EntityManager::GetInstance().Has<AudioComponent>(sound))
                    EntityManager::GetInstance().Get<AudioComponent>(sound).PlayAudioSound();
                EntityManager::GetInstance().RemoveEntity(sound);

                attacksoundonce = true;
            }
        }
        else
        {
            p_data->finishedExecution = true;
        }

    }

    void RatAttack_v2_0::StateCleanUp()
    {
        REMOVE_KEY_COLLISION_LISTENER(m_attackEventListener);
        REMOVE_KEY_COLLISION_LISTENER(m_attackStayEventListener);
    }

    void RatAttack_v2_0::StateExit(EntityID id)
    {
        // Cleanup attack-specific data here
        gameStateController = nullptr;
        std::cout << "RatAttack_v2_0::StateExit - Rat ID: " << id << " is exiting the attack state." << std::endl;



        p_data->hitCat = false;

    }

    void RatAttack_v2_0::RatHitCat(const Event<CollisionEvents>& r_TE)
    {
        GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->RatHitCat(p_data->myID, r_TE);
    }

    void RatScript_v2_0::RatHitCat(EntityID id, const Event<CollisionEvents>& r_TE)
    {
        // if cat has been checked before check the next event
        if (m_scriptData[id].hitCat) { return; }
        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if entity1 is rat or rat's attack and entity2 is cat
            if ((OTEE.Entity1 == m_scriptData[id].myID || OTEE.Entity1 == m_scriptData[id].attackTelegraphEntityID) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTEE.Entity2, OTEE.Entity1, id);
            }
            // check if entity2 is rat or rat's attack and entity1 is cat
            else if ((OTEE.Entity2 == m_scriptData[id].myID || OTEE.Entity2 == m_scriptData[id].attackTelegraphEntityID) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
            {
                // save the id of the cat that has been checked so that it wont be checked again
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTEE.Entity1, OTEE.Entity2, id);
            }
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if entity1 is rat or rat's attack and entity2 is cat
            if ((OTSE.Entity1 == m_scriptData[id].myID || OTSE.Entity1 == m_scriptData[id].attackTelegraphEntityID) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
            {
                // save the id of the cat that has been checked so that it wont be checked again
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTSE.Entity2, OTSE.Entity1, id);
            }
            // check if entity2 is rat or rat's attack and entity1 is cat
            else if ((OTSE.Entity2 == m_scriptData[id].myID || OTSE.Entity2 == m_scriptData[id].attackTelegraphEntityID) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
            {
                // save the id of the cat that has been checked so that it wont be checked again
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CheckFollowOrMain(m_scriptData[id].mainCatID, OTSE.Entity1, OTSE.Entity2, id);
            }
        }
    }

    void RatScript_v2_0::CheckFollowOrMain(EntityID mainCat, EntityID collidedCat, EntityID damagingID, EntityID rat)
    {
        try
        {
            auto& catFollowScript = *GETSCRIPTDATA(FollowScript, mainCat);
            // checks hit cat is the main cat
            if (collidedCat == mainCat)
            {
                int const& damage = (rat == m_scriptData[rat].myID) ? m_scriptData[rat].collisionDamage : m_scriptData[rat].attackDamage;
                try
                {
                    GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(mainCat, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
                    m_scriptData[rat].hitCat = true; // prevents double hits
                }
                catch (...) {}
            }
            // checks if hit cat is a following cat
            else if (catFollowScript.NumberOfFollower >= 2)
            {
                if (std::find(catFollowScript.FollowingObject.begin(), catFollowScript.FollowingObject.end(), collidedCat) != catFollowScript.FollowingObject.end())
                {
                    int const& damage = (damagingID == rat) ? m_scriptData[rat].collisionDamage : m_scriptData[rat].attackDamage;
                    try
                    {
                        GETSCRIPTINSTANCEPOINTER(CatScript)->LoseHP(mainCat, damage); // only the main cat should lose hp because the number of cat following will decrease based on its hp
                        m_scriptData[rat].hitCat = true; // prevents double hits
                    }
                    catch (...) {}
                }
            }
        }
        catch (...) {}
    }

    void RatAttack_v2_0::OnTriggerEnterForAttack(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }

        if (r_TE.GetType() == CollisionEvents::OnTriggerEnter)
        {
            OnTriggerEnterEvent OTEE = dynamic_cast<OnTriggerEnterEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTEE.Entity1 == p_data->attackRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTEE.Entity2 == p_data->attackRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTEE.Entity1);
            }
        }
        else if (r_TE.GetType() == CollisionEvents::OnTriggerStay)
        {
            OnTriggerStayEvent OTSE = dynamic_cast<OnTriggerStayEvent const&>(r_TE);
            // check if entity1 is the rat's detection collider and entity2 is cat
            if ((OTSE.Entity1 == p_data->attackRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity2) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity2).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity2);
            }
            // check if entity2 is the rat's detection collider and entity1 is cat
            else if ((OTSE.Entity2 == p_data->attackRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTSE.Entity1) &&
                EntityManager::GetInstance().Get<EntityDescriptor>(OTSE.Entity1).name.find("Cat") != std::string::npos)
            {
                GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatEntered(p_data->myID, OTSE.Entity1);
            }
        }
    }

    void RatAttack_v2_0::OnTriggerStayForAttack(const Event<CollisionEvents>& r_TE)
    {
        if (!p_data) { return; }

        OnTriggerExitEvent OTEE = dynamic_cast<OnTriggerExitEvent const&>(r_TE);
        // check if entity1 is the rat's detection collider and entity2 is cat
        if ((OTEE.Entity1 == p_data->attackRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity2) &&
            EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity2).name.find("Cat") != std::string::npos)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity2);
        }
        // check if entity2 is the rat's detection collider and entity1 is cat
        else if ((OTEE.Entity2 == p_data->attackRadiusId) && EntityManager::GetInstance().Has<EntityDescriptor>(OTEE.Entity1) &&
            EntityManager::GetInstance().Get<EntityDescriptor>(OTEE.Entity1).name.find("Cat") != std::string::npos)
        {
            GETSCRIPTINSTANCEPOINTER(RatScript_v2_0)->CatExited(p_data->myID, OTEE.Entity1);
        }
    }

} // namespace PE
