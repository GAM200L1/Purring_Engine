/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SerializationManager.cpp
 \date     25-09-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file contains the implementation of the SerializationManager class.
           The SerializationManager is responsible for serializing and deserializing entities
           to and from JSON format. It offers an interface for saving serialized entities to files
           and loading them back. The manager also handles the initialization of various component
           types during the deserialization process.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "SerializationManager.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "Math/MathCustom.h"
#include <filesystem>


std::string SerializationManager::OpenFileExplorer()
{
    OPENFILENAME ofn;
    wchar_t szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        std::wstring wfp = ofn.lpstrFile;
        int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wfp.c_str(), -1, nullptr, 0, nullptr, nullptr);

        if (requiredSize > 0)
        {
            std::string fp(requiredSize, '\0');
            if (WideCharToMultiByte(CP_UTF8, 0, wfp.c_str(), -1, &fp[0], requiredSize, nullptr, nullptr) > 0)
            {
                return fp;
            }
        }
    }

    return "";
}

    nlohmann::json SerializationManager::SerializeAllEntities()
    {
        nlohmann::json allEntitiesJson;

        // Use GetEntitiesInPool with the "ALL" constant to get all entity IDs - JW and Hans
        std::vector<EntityID>& allEntityIds = PE::EntityManager::GetInstance().GetEntitiesInPool(ALL);

        for (const auto& entityId : allEntityIds)
        {
            nlohmann::json entityJson = SerializeEntity(static_cast<int>(entityId));
            allEntitiesJson["Entities"].push_back(entityJson);
        }

        return allEntitiesJson;
    }

void SerializationManager::DeserializeAllEntities(const nlohmann::json& r_j)
{
    if (r_j.contains("Entities"))
    {
        for (const auto& entityJson : r_j["Entities"])
        {
            DeserializeEntity(entityJson);
        }
    }
}

void SerializationManager::SaveAllEntitiesToFile(const std::filesystem::path& filepath)
{
    nlohmann::json allEntitiesJson = SerializeAllEntities();

    std::ofstream outFile(filepath);
    if (outFile)
    {
        outFile << allEntitiesJson.dump(4);
        outFile.close();
    }
    else
    {
        std::cerr << "Could not open the file for writing: " << filepath << std::endl;
    }
}

void SerializationManager::LoadAllEntitiesFromFile(const std::filesystem::path& filepath)
{
    if (!std::filesystem::exists(filepath))
    {
        std::cerr << "File does not exist: " << filepath << std::endl;
        return;
    }

    std::ifstream inFile(filepath);
    if (inFile)
    {
        nlohmann::json allEntitiesJson;
        inFile >> allEntitiesJson;
        DeserializeAllEntities(allEntitiesJson);
        inFile.close();
    }
    else
    {
        std::cerr << "Could not open the file for reading: " << filepath << std::endl;
    }
}

nlohmann::json SerializationManager::SerializeEntity(int entityId)
{
    //PE::EntityManager* entityManager = &PE::EntityManager::GetInstance();
    //EntityID eID = static_cast<EntityID>(entityId);

    nlohmann::json j;
    StructEntity& entity = m_entities[entityId];

    j["Entity"]["name"] = entity.m_name;

    for (const auto& [key, val] : entity.m_data)
    {
        if (val.type() == typeid(int))
        {
            j["Entity"]["data"][key]["type"] = "int";
            j["Entity"]["data"][key]["value"] = std::any_cast<int>(val);
        }
        else if (val.type() == typeid(float))
        {
            j["Entity"]["data"][key]["type"] = "float";
            j["Entity"]["data"][key]["value"] = std::any_cast<float>(val);
        }
        else if (val.type() == typeid(std::string))
        {
            j["Entity"]["data"][key]["type"] = "string";
            j["Entity"]["data"][key]["value"] = std::any_cast<std::string>(val);
        }
        else if (val.type() == typeid(std::vector<int>))
        {
            j["Entity"]["data"][key]["type"] = "vector<int>";
            j["Entity"]["data"][key]["value"] = std::any_cast<std::vector<int>>(val);
        }
    }

    SerializeComponent<PE::Transform>(entityId, "Transform", j);
    SerializeComponent<PE::Graphics::Renderer>(entityId, "Renderer", j);
    SerializeComponent<PE::RigidBody>(entityId, "RigidBody", j);
    SerializeComponent<PE::Collider>(entityId, "Collider", j);
    SerializeComponent<PE::Graphics::Camera>(entityId, "Camera", j);


    return j; 
}

size_t SerializationManager::DeserializeEntity(const nlohmann::json& r_j)
{
    StructEntity entity;


    if (m_initializeComponent.empty())
        LoadLoaders();

    EntityID id = MAXSIZE_T;
    if (r_j.contains("Entity"))
    {
        const auto& entityJson = r_j["Entity"];


        id = PE::EntityFactory::GetInstance().CreateEntity();
        for (const auto& t : r_j["Entity"].items())
        {
            // to change?
            if (t.key() == "name")
            {
                entity.m_name = entityJson["name"];
            }
            else
            {
                for (const auto& data : r_j["Entity"][t.key()].items())
                {
                    const std::string k = data.key();
                    std::invoke(m_initializeComponent[k], this, id, r_j);
                }
                // // Deserialize components
            }
        }
    }
    return id;
}

void SerializationManager::SaveToFile(const std::filesystem::path& filepath, int entityId)
{
    nlohmann::json serializedEntity = SerializeEntity(entityId);
    std::ofstream outFile(filepath);
    if (outFile)
    {
        outFile << serializedEntity.dump(4);
        outFile.close();
    }
    else
    {
        std::cerr << "Could not open the file for writing: " << filepath << std::endl;
    }
}

size_t SerializationManager::LoadFromFile(const std::filesystem::path& filepath)
{
    if (!std::filesystem::exists(filepath))
    {
        std::cerr << "File does not exist: " << filepath << std::endl;
        return MAXSIZE_T;
    }

    std::ifstream inFile(filepath);
    if (inFile)
    {
        nlohmann::json j;
        inFile >> j;
        inFile.close();
        return DeserializeEntity(j);
    }
    else
    {
        std::cerr << "Could not open the file for reading: " << filepath << std::endl;
        return MAXSIZE_T;
    }
}

void SerializationManager::LoadLoaders()
{
    m_initializeComponent.emplace("RigidBody", &SerializationManager::LoadRigidBody);
    m_initializeComponent.emplace("Collider", &SerializationManager::LoadCollider);
    m_initializeComponent.emplace("Transform", &SerializationManager::LoadTransform);
    m_initializeComponent.emplace("Renderer", &SerializationManager::LoadRenderer);
    m_initializeComponent.emplace("Camera", &SerializationManager::LoadCamera);

}

bool SerializationManager::LoadRigidBody(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::RigidBody rb;
    rb.SetMass(r_json["Entity"]["components"]["RigidBody"]["mass"].get<float>());
    rb.SetType(static_cast<EnumRigidBodyType>(r_json["Entity"]["components"]["RigidBody"]["type"].get<int>()));
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::RigidBody>(), static_cast<void*>(&rb));
    return true;
}

bool SerializationManager::LoadCollider(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Collider col;
    std::string type = r_json["Entity"]["components"]["Collider"]["type"].get<std::string>();
    if (type == "CircleCollider")
    {
        col.colliderVariant = PE::CircleCollider();
    }
    else
    {
        col.colliderVariant = PE::AABBCollider();
    }
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Collider>(), static_cast<void*>(&col));
    return true;
}

bool SerializationManager::LoadTransform(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Transform trans;
    trans.height = r_json["Entity"]["components"]["Transform"]["height"].get<float>();
    trans.width = r_json["Entity"]["components"]["Transform"]["width"].get<float>();
    trans.orientation = r_json["Entity"]["components"]["Transform"]["orientation"].get<float>();
    trans.position = PE::vec2{ r_json["Entity"]["components"]["Transform"]["position"]["x"].get<float>(), r_json["Entity"]["components"]["Transform"]["position"]["y"].get<float>() };
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Transform>(), static_cast<void*>(&trans));
    return true;
}

bool SerializationManager::LoadRenderer(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Graphics::Renderer ren;
    ren.SetColor(r_json["Entity"]["components"]["Renderer"]["Color"]["r"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["g"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["b"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["a"].get<float>());
    ren.SetTextureKey(r_json["Entity"]["components"]["Renderer"]["TextureKey"].get<std::string>());
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::Renderer>(), static_cast<void*>(&ren));
    return true;
}

bool SerializationManager::LoadCamera(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Graphics::Camera cam;
    cam.SetMagnification(r_json["Entity"]["components"]["Camera"]["magnification"].get<float>());
    cam.SetViewDimensions
    (
        r_json["Entity"]["components"]["Camera"]["viewportWidth"].get<float>(),
        r_json["Entity"]["components"]["Camera"]["viewportHeight"].get<float>()
    );
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::Camera>(), static_cast<void*>(&cam));
    return true;
}