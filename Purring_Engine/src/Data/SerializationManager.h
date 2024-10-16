/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SerializationManager.h
 \date     25-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu
 \par      code %:     70%
 \par      changes:    Majority of code and structure 

 \co-author            FOONG Jun Wei 
 \par      email:      f.junwei\@digipen.edu 
 \par      code %:     30% 
 \par      changes:    LoadAnimationComponent(), LoadTextComponent(), 
                       LoadScriptComponent()

 \brief	   This file contains the implementation of the SerializationManager class.
           The SerializationManager is responsible for serializing and deserializing entities
           to and from JSON format. It offers an interface for saving serialized entities to files
           and loading them back. The manager also handles the initialization of various component
           types during the deserialization process.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */

#include "json.hpp"
#include "Math/Transform.h"
#include "Math/MathCustom.h"
#include "Physics/RigidBody.h"
#include "Physics/Colliders.h"
#include "Graphics/Renderer.h"
#include "Graphics/GUIRenderer.h"
#include "Data/JsonUtils.h"
#include "GUISystem.h"
#include <type_traits>
#include "AudioManager/AudioComponent.h"

struct StructPlayerStats
{
    int m_health;
    int m_level;
    float m_experience;
    std::string m_playerName;
};

struct StructEntity
{
    std::string m_name;                                   // Entity name
    std::unordered_map<std::string, std::any> m_data;     // Variable names and their data

    // Additional data members
    int m_id{ 0 };
    int m_someInt;
    float m_someFloat;
    double m_someDouble;
    char m_someChar;
    bool m_someBool;
    std::string m_someString;
};

class SerializationManager
{
    // ----- Public Methods ----- //
public:
    /*!***********************************************************************************
     \brief Opens a file explorer and returns the selected file path as a string.

     \param[in] type  The file type to filter for.
    *************************************************************************************/
    std::string OpenFileExplorer(std::string const& type = ".json");

    /*!***********************************************************************************
     \brief Opens a file explorer and returns the selected file path as a string.
            This version is used mainly to save a file with a potentially not existing
            filename.
    \param[in] type  The file type to filter for.
    *************************************************************************************/
    std::string OpenFileExplorerRequestPath(std::string const& type = ".json");

    /*!***********************************************************************************
     \brief Serialize all entities in the scene into a single JSON object.
    *************************************************************************************/
    nlohmann::json SerializeAllEntities();

    /*!***********************************************************************************
     \brief Deserialize a JSON object to recreate all entities in the scene.
    *************************************************************************************/
    void DeserializeAllEntities(const nlohmann::json& r_j);

    /*!***********************************************************************************
     \brief Save the serialized JSON of all entities to a file with the given filename.
    *************************************************************************************/
    void SerializeScene(std::string const& fileName, bool fp = false);

    void DeserializeScene(std::string const& r_scenePath);

    /*!***********************************************************************************
     \brief Serialize the entity with the given ID to a JSON object.
    *************************************************************************************/
    nlohmann::json SerializeEntity(int entityId);

    /*!***********************************************************************************
     \brief Serialize the entity with the given ID to a JSON object as a prefab
    *************************************************************************************/
    nlohmann::json SerializeEntityPrefab(int entityId);

    /*!***********************************************************************************
     \brief Serialize the entity with the given ID to a JSON object as a prefab composite
    *************************************************************************************/
    nlohmann::json SerializeEntityComposite(int entityID);

    /*!***********************************************************************************
     \brief Serialize the entity with the given ID to a JSON object as a prefab composite
    *************************************************************************************/
    nlohmann::json SerializePrefabComposite();

    /*!***********************************************************************************
     \brief Deserialize a JSON object to create an entity, returning its ID.
    *************************************************************************************/
    size_t DeserializeEntity(const nlohmann::json& r_j);

    /*!***********************************************************************************
     \brief Save the serialized entity to a file.
    *************************************************************************************/
    void SaveToFile(const std::filesystem::path& filepath, int entityId);

    /*!***********************************************************************************
    \brief Save animation data to file

    \param[in] filepath  The path to the file to save to.
    \param[in] serializedData  The data to save.
    *************************************************************************************/
    void SerializeAnimation(const std::filesystem::path& filepath, const nlohmann::json& serializedData);

    /*!***********************************************************************************
     \brief Load an entity from a serialized file, returning its ID.

     \param[in] filename  The path to the file to load from.
     \param[in] fp  Whether to use the file path as the entity name.

     \return ID of the entity loaded.
    *************************************************************************************/
    size_t LoadFromFile(std::string const& filename, bool fp = false);

    /*!***********************************************************************************
    \brief Load an animation from a serialized file.

    \param[in] filename  The path to the file to load from.
    \return nlohmann::json  The loaded data.
    *************************************************************************************/
    nlohmann::json DeserializeAnimation(const std::filesystem::path& filepath);

    /*!***********************************************************************************
    \brief Save animation data to file

    \param[in] filepath  The path to the file to save to.
    \param[in] serializedData  The data to save.
    *************************************************************************************/
    void SaveMetaDataToFile(const std::filesystem::path& filepath, const nlohmann::json& serializedData);

    /*!***********************************************************************************
    \brief Load an animation from a serialized file.

    \param[in] filename  The path to the file to load from.
    \return nlohmann::json  The loaded data.
    *************************************************************************************/
    nlohmann::json LoadMetaDataFromFile(const std::filesystem::path& filepath);

    /*!************************************************************************
     \brief Serializes an entity's component to JSON.

     Serializes the ComponentType of the specified entity and adds it to the
     provided JSON object with the given key.

     \tparam ComponentType Type of component to serialize.
     \param entityId Entity ID.
     \param jsonKey Key for the serialized data.
     \param json JSON object for output.
    *************************************************************************/
    template<typename ComponentType>
    void SerializeComponent(int entityId, const std::string& jsonKey, nlohmann::json& json);


    size_t CreationHelper(const nlohmann::json& r_j);

    /*!***********************************************************************************
    \brief Loads a prefab from file

    \param[in] r_json  The path to the file to save to.
    *************************************************************************************/
    size_t CreateEntityFromPrefab(std::string const& r_filePath);

    // ----- Private Methods ----- //
private:
    /*!***********************************************************************************
     \brief Load the serialization functions for different components.
    *************************************************************************************/
    void LoadLoaders();
    /*!***********************************************************************************
     \brief Load the Transform component from JSON.
    *************************************************************************************/
    bool LoadTransform(const size_t& r_id, const nlohmann::json& r_json);
    /*!***********************************************************************************
     \brief Load the Renderer component from JSON.
    *************************************************************************************/
    bool LoadRenderer(const size_t& r_id, const nlohmann::json& r_json);
    /*!***********************************************************************************
     \brief Load the RigidBody component from JSON.
    *************************************************************************************/
    bool LoadRigidBody(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Collider component from JSON.
    *************************************************************************************/
    bool LoadCollider(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the camera component from JSON.
    *************************************************************************************/
    bool LoadCamera(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the GUI component from JSON.
    *************************************************************************************/
    bool LoadGUI(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Renderer component from JSON.
    *************************************************************************************/
    bool LoadGUIRenderer(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the EntityDescriptor component from JSON.
    *************************************************************************************/
    bool LoadEntityDescriptor(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Animation component from JSON.
    *************************************************************************************/
    bool LoadAnimationComponent(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Text component from JSON.
    *************************************************************************************/
    bool LoadTextComponent(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Text component from JSON.
    *************************************************************************************/
    bool LoadCanvasComponent(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Script component from JSON.
    *************************************************************************************/
    bool LoadScriptComponent(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Audio component from JSON.
    *************************************************************************************/
    bool LoadAudioComponent(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Transform component from JSON.
    *************************************************************************************/
    bool LoadGUISlider(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Load the Particle Emitter component from JSON.
    *************************************************************************************/
    bool LoadParticleEmitter(const size_t& r_id, const nlohmann::json& r_json);


    // ----- Private Methods ----- //
private:
    /*!***********************************************************************************
     \brief Map of entities identified by integer IDs.
    *************************************************************************************/
    std::unordered_map<int, StructEntity> m_entities;              // Store entities with integer IDs

    /*!***********************************************************************************
     \brief Default struct name used for serialization.
    *************************************************************************************/
    std::string m_structName = "Entity";                           // Default struct name for serialization

    /*!***********************************************************************************
     \brief Function pointer map for initializing components.
    *************************************************************************************/
    typedef bool(SerializationManager::* FnptrVoidptrLoad)(const size_t& r_id, const nlohmann::json& r_json);

    /*!***********************************************************************************
     \brief Map of function pointers for initializing components.
    *************************************************************************************/
    std::map<std::string, FnptrVoidptrLoad> m_initializeComponent;
};

/*!*****************************************************************************
 \brief Serializes an entity's component into a JSON object.

 Serializes the specified component of the given entity, adding it to the provided
 JSON object with the specified key.

 \tparam ComponentType Type of the component to serialize.
 \param entityId ID of the entity.
 \param jsonKey Key for the serialized component in the JSON object.
 \param json JSON object to store the serialized component.
*******************************************************************************/
template<typename ComponentType>
void SerializationManager::SerializeComponent(int entityId, const std::string& jsonKey, nlohmann::json& json)
{
    PE::EntityManager& entityManager = PE::EntityManager::GetInstance();
    if (entityManager.Has(static_cast<EntityID>(entityId), entityManager.GetComponentID<ComponentType>()))
    {
        ComponentType& component = entityManager.Get<ComponentType>(entityId);

        if constexpr (std::is_same<ComponentType, PE::AudioComponent>::value)
        {
            json["Entity"]["components"][jsonKey] = component.ToJson();
        }
        else
        {
            json["Entity"]["components"][jsonKey] = component.ToJson(static_cast<EntityID>(entityId));
        }
    }
}

