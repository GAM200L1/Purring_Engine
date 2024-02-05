/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     SerializationManager.cpp
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

/*                                                                                                          includes
--------------------------------------------------------------------------------------------------------------------- */
#include "prpch.h"
#include "SerializationManager.h"
#include "ECS/EntityFactory.h"
#include "ECS/Entity.h"
#include "Math/MathCustom.h"
#include <filesystem>
#include "Graphics/CameraManager.h"
#include "Logic/LogicSystem.h"
#include "Logic/PlayerControllerScript.h"
#include "Graphics/Text.h"
#include "Math/MathCustom.h"
#include "GUI/Canvas.h"
#include "ResourceManager/ResourceManager.h"

// RTTR
#include <rttr/variant.h>
#include <rttr/type.h>

const std::wstring wjsonExt = L".json";

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

std::string SerializationManager::OpenFileExplorerRequestPath()
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
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        std::wstring wfp = ofn.lpstrFile;
        // check for extention
        for (size_t i{ wfp.length() - 5 }, j{}; i < wfp.length(); ++i, ++j)
        {
            // add the extention if not match
            if (wfp[i] != wjsonExt[j])
            {
                wfp.append(wjsonExt);
                break;
            }
        }
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

        // Serialize all loaded resources metafile
        //std::vector<std::string> allResources = 

        // Use GetEntitiesInPool with the "ALL" constant to get all entity IDs - JW and Hans
        std::vector<EntityID>& allEntityIds = PE::EntityManager::GetInstance().GetEntitiesInPool(ALL);

        for (const auto& entityId : allEntityIds)
        {
            if (entityId != PE::Graphics::CameraManager::GetUiCameraId() && PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(entityId).SaveEntity())
            {
                nlohmann::json entityJson = SerializeEntity(static_cast<int>(entityId));
                allEntitiesJson["Entities"].push_back(entityJson);
            }

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

void SerializationManager::SaveAllEntitiesToFile(std::string const& filename, bool fp)
{
    nlohmann::json allEntitiesJson = SerializeAllEntities();

    std::filesystem::path filepath;

    // if using filepath
    if (fp)
    {
        filepath = filename;
    }
    else
    {
        filepath = std::string{ "../Assets/Scenes/" } + filename;
    }

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

nlohmann::json SerializationManager::SerializeEntity(int entityId)
{
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
    SerializeComponent<PE::GUIButton>(entityId, "GUI", j);
    //SerializeComponent<PE::GUISlider>(entityId, "GUISlider", j);
    SerializeComponent<PE::Graphics::GUIRenderer>(entityId, "GUIRenderer", j); 
    SerializeComponent<PE::EntityDescriptor>(entityId, "EntityDescriptor", j);
    SerializeComponent<PE::ScriptComponent>(entityId, "ScriptComponent", j);
    SerializeComponent<PE::AnimationComponent>(entityId, "AnimationComponent", j);
    SerializeComponent<PE::TextComponent>(entityId, "TextComponent", j);
    SerializeComponent<PE::AudioComponent>(entityId, "AudioComponent", j);
    SerializeComponent<PE::Canvas>(entityId, "Canvas", j);
    SerializeComponent<PE::GUISlider>(entityId, "GUISlider", j);


    return j; 
}

nlohmann::json SerializationManager::SerializeEntityPrefab(int entityId)
{
    PE::EntityDescriptor tmp;
    tmp.name = PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(static_cast<EntityID>(entityId)).name;
    if (PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(static_cast<EntityID>(entityId)).prefabType == "")
        tmp.prefabType = tmp.name;
    else
        tmp.prefabType = PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(static_cast<EntityID>(entityId)).prefabType;

    std::swap(PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(static_cast<EntityID>(entityId)), tmp);
    nlohmann::json ret = SerializeEntity(entityId);
    std::swap(PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(static_cast<EntityID>(entityId)), tmp);
    return ret;
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
        PE::EntityDescriptor desc = PE::EntityDescriptor::Deserialize(r_j["Entity"]["components"]["EntityDescriptor"]);

        id = PE::EntityManager::GetInstance().NewEntity(desc.oldID);
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

void SerializationManager::SaveAnimationToFile(const std::filesystem::path& filepath, const nlohmann::json& serializedData)
{
    std::ofstream outFile(filepath);
    if (outFile)
    {
        outFile << serializedData.dump(4);
        outFile.close();
    }
    else
    {
        std::cerr << "Could not open the file for writing: " << filepath << std::endl;
    }
}

size_t SerializationManager::LoadFromFile(std::string const& filename, bool fp)
{
    std::filesystem::path filepath;
    
    // if using filepath
    if (fp)
    {
        filepath = filename;
    }
    else
    {
        filepath = std::string{ "../Assets/Prefabs/" } + filename;
    }
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

nlohmann::json SerializationManager::LoadAnimationFromFile(const std::filesystem::path& filepath)
{
    nlohmann::json loadedData;
    std::ifstream inFile(filepath);
    if (inFile)
    {
        inFile >> loadedData;
        inFile.close();
    }
    else {
        std::cerr << "Could not open the file for reading: " << filepath << std::endl;
    }
    return loadedData;
}



void SerializationManager::LoadLoaders()
{
    m_initializeComponent.emplace("Transform", &SerializationManager::LoadTransform);
    m_initializeComponent.emplace("Renderer", &SerializationManager::LoadRenderer);
    m_initializeComponent.emplace("RigidBody", &SerializationManager::LoadRigidBody);
    m_initializeComponent.emplace("Collider", &SerializationManager::LoadCollider);
    m_initializeComponent.emplace("Camera", &SerializationManager::LoadCamera);
    m_initializeComponent.emplace("GUI", &SerializationManager::LoadGUI);
    m_initializeComponent.emplace("GUIRenderer", &SerializationManager::LoadGUIRenderer);
    m_initializeComponent.emplace("EntityDescriptor", &SerializationManager::LoadEntityDescriptor);
    m_initializeComponent.emplace("ScriptComponent", &SerializationManager::LoadScriptComponent);
    m_initializeComponent.emplace("AnimationComponent", &SerializationManager::LoadAnimationComponent);
    m_initializeComponent.emplace("TextComponent", &SerializationManager::LoadTextComponent);
    m_initializeComponent.emplace("AudioComponent", &SerializationManager::LoadAudioComponent);
    m_initializeComponent.emplace("Canvas", &SerializationManager::LoadCanvasComponent);
    m_initializeComponent.emplace("GUISlider", &SerializationManager::LoadGUISlider);

}

bool SerializationManager::LoadTransform(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Transform trans;
    trans.height = r_json["Entity"]["components"]["Transform"]["height"].get<float>();
    trans.width = r_json["Entity"]["components"]["Transform"]["width"].get<float>();
    trans.orientation = r_json["Entity"]["components"]["Transform"]["orientation"].get<float>();
    trans.position = PE::vec2{ r_json["Entity"]["components"]["Transform"]["position"]["x"].get<float>(), r_json["Entity"]["components"]["Transform"]["position"]["y"].get<float>() };
    if (r_json["Entity"]["components"]["Transform"].contains("relativeposition"))
        trans.relPosition = PE::vec2{ r_json["Entity"]["components"]["Transform"]["relativeposition"]["x"].get<float>(), r_json["Entity"]["components"]["Transform"]["relativeposition"]["y"].get<float>() };
    if (r_json["Entity"]["components"]["Transform"].contains("relorientation"))
        trans.relOrientation = r_json["Entity"]["components"]["Transform"]["relorientation"].get<float>();
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Transform>(), static_cast<void*>(&trans));
    return true;
}

bool SerializationManager::LoadRenderer(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Graphics::Renderer ren;
    ren.SetColor(r_json["Entity"]["components"]["Renderer"]["Color"]["r"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["g"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["b"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["a"].get<float>());
    ren.SetTextureKey(r_json["Entity"]["components"]["Renderer"]["TextureKey"].get<std::string>());
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::Renderer>(), static_cast<void*>(&ren));

    // Load resource
    PE::ResourceManager::GetInstance().AddTextureKeyToLoad(ren.GetTextureKey());

    return true;
}

bool SerializationManager::LoadRigidBody(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::RigidBody rb;
    rb.SetMass(r_json["Entity"]["components"]["RigidBody"]["mass"].get<float>());
    rb.SetType(static_cast<EnumRigidBodyType>(r_json["Entity"]["components"]["RigidBody"]["type"].get<int>()));
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::RigidBody>(), static_cast<void*>(&rb));
    return true;
}

void LoadHelper(PE::CircleCollider& r_col, const nlohmann::json& r_json)
{
    if (r_json.contains("positionOffset"))
        r_col.positionOffset = PE::vec2{r_json["positionOffset"]["x"].get<float>(), r_json["positionOffset"]["y"].get<float>() };
    if (r_json.contains("scaleOffset"))
        r_col.scaleOffset = r_json["scaleOffset"].get<float>();
}
void LoadHelper(PE::AABBCollider& r_col, const nlohmann::json& r_json)
{
    if (r_json.contains("positionOffset"))
        r_col.positionOffset = PE::vec2{ r_json["positionOffset"]["x"].get<float>(), r_json["positionOffset"]["y"].get<float>() };
    if (r_json.contains("scaleOffset"))
        r_col.scaleOffset = PE::vec2{ r_json["scaleOffset"]["x"].get<float>(), r_json["scaleOffset"]["y"].get<float>() };
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
    std::visit([&](auto& col1)
    {
            LoadHelper(col1, r_json["Entity"]["components"]["Collider"]["data"]);
    }, col.colliderVariant);

    if (r_json["Entity"]["components"]["Collider"].contains("isTrigger"))
        col.isTrigger = r_json["Entity"]["components"]["Collider"]["isTrigger"].get<bool>();
    if (r_json["Entity"]["components"]["Collider"].contains("collisionLayerIndex"))
        col.collisionLayerIndex = r_json["Entity"]["components"]["Collider"]["collisionLayerIndex"].get<unsigned>();
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Collider>(), static_cast<void*>(&col));
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
    if (r_json["Entity"]["components"]["Camera"].contains("ismaincamera"))
        cam.SetMainCamera(r_json["Entity"]["components"]["Camera"]["ismaincamera"].get<bool>());
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::Camera>(), static_cast<void*>(&cam));
    return true;
}

bool SerializationManager::LoadGUI(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::GUIButton gui;

    if (r_json["Entity"]["components"].contains("GUI"))
    {    
        if (r_json["Entity"]["components"]["GUI"].contains("m_onClicked"))
            gui.m_onClicked = r_json["Entity"]["components"]["GUI"]["m_onClicked"].get<std::string>();
        if (r_json["Entity"]["components"]["GUI"].contains("m_onHovered"))
            gui.m_onHovered = r_json["Entity"]["components"]["GUI"]["m_onHovered"].get<std::string>();
        if (r_json["Entity"]["components"]["GUI"].contains("m_UIType"))
            gui.m_UIType = static_cast<PE::UIType>(r_json["Entity"]["components"]["GUI"]["m_UIType"].get<int>());
        if (r_json["Entity"]["components"]["GUI"].contains("disabled"))
            gui.disabled = r_json["Entity"]["components"]["GUI"]["disabled"].get<bool>();
        if (r_json["Entity"]["components"]["GUI"].contains("m_defaultTexture"))
            gui.m_defaultTexture = r_json["Entity"]["components"]["GUI"]["m_defaultTexture"].get<std::string>();
        if (r_json["Entity"]["components"]["GUI"].contains("m_hoveredTexture"))
            gui.m_hoveredTexture = r_json["Entity"]["components"]["GUI"]["m_hoveredTexture"].get<std::string>();
        if (r_json["Entity"]["components"]["GUI"].contains("m_pressedTexture"))
            gui.m_pressedTexture = r_json["Entity"]["components"]["GUI"]["m_pressedTexture"].get<std::string>();
        if (r_json["Entity"]["components"]["GUI"].contains("m_disabledTexture"))
            gui.m_disabledTexture = r_json["Entity"]["components"]["GUI"]["m_disabledTexture"].get<std::string>();

        if (r_json["Entity"]["components"]["GUI"].contains("m_defaultColor"))
            gui.m_defaultColor = PE::vec4(
            r_json["Entity"]["components"]["GUI"]["m_defaultColor"][0].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_defaultColor"][1].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_defaultColor"][2].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_defaultColor"][3].get<float>()
        );

        if (r_json["Entity"]["components"]["GUI"].contains("m_hoveredColor"))
            gui.m_hoveredColor = PE::vec4(
            r_json["Entity"]["components"]["GUI"]["m_hoveredColor"][0].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_hoveredColor"][1].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_hoveredColor"][2].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_hoveredColor"][3].get<float>()
        );
        if (r_json["Entity"]["components"]["GUI"].contains("m_pressedColor"))
            gui.m_pressedColor = PE::vec4(
            r_json["Entity"]["components"]["GUI"]["m_pressedColor"][0].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_pressedColor"][1].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_pressedColor"][2].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_pressedColor"][3].get<float>()
        );
        if (r_json["Entity"]["components"]["GUI"].contains("m_disabledColor"))
            gui.m_disabledColor = PE::vec4(
            r_json["Entity"]["components"]["GUI"]["m_disabledColor"][0].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_disabledColor"][1].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_disabledColor"][2].get<float>(),
            r_json["Entity"]["components"]["GUI"]["m_disabledColor"][3].get<float>()
        );
    }
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::GUIButton>(), static_cast<void*>(&gui));
    return true;
}


bool SerializationManager::LoadGUIRenderer(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Graphics::GUIRenderer guiren;
    guiren.SetColor(r_json["Entity"]["components"]["GUIRenderer"]["Color"]["r"].get<float>(), r_json["Entity"]["components"]["GUIRenderer"]["Color"]["g"].get<float>(), r_json["Entity"]["components"]["GUIRenderer"]["Color"]["b"].get<float>(), r_json["Entity"]["components"]["GUIRenderer"]["Color"]["a"].get<float>());
    guiren.SetTextureKey(r_json["Entity"]["components"]["GUIRenderer"]["TextureKey"].get<std::string>());
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Graphics::GUIRenderer>(), static_cast<void*>(&guiren));
    
    // Load resource
    PE::ResourceManager::GetInstance().AddTextureKeyToLoad(guiren.GetTextureKey());

    return true;
}

bool SerializationManager::LoadEntityDescriptor(const EntityID& r_id, const nlohmann::json& r_json)
{
    // Deserialize EntityDescriptor from the json object
    PE::EntityDescriptor descriptor = PE::EntityDescriptor::Deserialize(r_json["Entity"]["components"]["EntityDescriptor"]);
    if (descriptor.oldID == ULLONG_MAX)
        descriptor.oldID = PE::EntityManager::GetInstance().Get<PE::EntityDescriptor>(r_id).oldID;
    
    // Pass the descriptor to the EntityFactory to create/update the EntityDescriptor component for the entity with id 'r_id'
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::EntityDescriptor>(), static_cast<void*>(&descriptor));

    return true;
}

bool SerializationManager::LoadAnimationComponent(const size_t& r_id, const nlohmann::json& r_json)
{
    PE::AnimationComponent comp{ (PE::AnimationComponent().Deserialize(r_json["Entity"]["components"]["AnimationComponent"])) };
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::AnimationComponent>(),
        static_cast<void*>(&comp));

    // load resource
    PE::ResourceManager::GetInstance().AddAnimationKeyToLoad(comp.GetAnimationID());
   // PE::ResourceManager::GetInstance().AddTextureKeyToLoad(PE::ResourceManager::GetInstance().GetAnimation(comp.GetAnimationID())->GetSpriteSheetKey());
    return true;
}

bool SerializationManager::LoadTextComponent(const size_t& r_id, const nlohmann::json& r_json)
{
    PE::TextComponent comp{ (PE::TextComponent().Deserialize(r_json["Entity"]["components"]["TextComponent"])) };
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::TextComponent>(),
        static_cast<void*>(&comp));

    // load resource
    PE::ResourceManager::GetInstance().AddFontKeyToLoad(comp.GetFontKey());
    return true;
}

bool SerializationManager::LoadCanvasComponent(const size_t& r_id, const nlohmann::json& r_json)
{
    if (r_json["Entity"]["components"].contains("Canvas"))
    {

        PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::Canvas>(), 
            static_cast<void*>(&(PE::Canvas().Deserialize(r_json["Entity"]["components"]["Canvas"]))));
        return true;
    }
    return false;
}

bool SerializationManager::LoadScriptComponent(const size_t& r_id, const nlohmann::json& r_json)
{
    PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::ScriptComponent>(),
        static_cast<void*>(&(PE::ScriptComponent().Deserialize(r_json["Entity"]["components"]["ScriptComponent"]))));

    for (const auto& k : r_json["Entity"]["components"]["ScriptComponent"].items())
    {
        auto str = k.key().c_str();
        if (PE::LogicSystem::m_scriptContainer.count(str))
        {
            rttr::instance inst = PE::LogicSystem::m_scriptContainer.at(str)->GetScriptData(r_id);
            if (k.value().contains("data"))
            {
                const auto& data = k.value()["data"];
                if (inst.is_valid())
                {
                    for (auto& prop : rttr::type::get_by_name(str).get_properties())
                    {
                        if (!data.contains(prop.get_name().to_string().c_str()))
                            continue;
                        if (prop.get_type().get_name() == "float")
                        {
                            float val = data[prop.get_name().to_string().c_str()].get<float>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "enumPE::PlayerState")
                        {
                            PE::PlayerState val = data[prop.get_name().to_string().c_str()].get<PE::PlayerState>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "int")
                        {
                            int val = data[prop.get_name().to_string().c_str()].get<int>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "unsigned__int64")
                        {
                            EntityID val = data[prop.get_name().to_string().c_str()].get<EntityID>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "bool")
                        {
                            bool val = data[prop.get_name().to_string().c_str()].get<bool>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "classstd::vector<unsigned__int64,classstd::allocator<unsigned__int64> >")
                        {
                            std::vector<EntityID> val = data[prop.get_name().to_string().c_str()].get<std::vector<EntityID>>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "structPE::vec2")
                        {
                            PE::vec2 val;
                            val.x = data[prop.get_name().to_string().c_str()]["x"].get<float>();
                            val.y = data[prop.get_name().to_string().c_str()]["x"].get<float>();
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "classstd::vector<structPE::vec2,classstd::allocator<structPE::vec2> >")
                        {
                            std::vector<PE::vec2> val;

                            for (size_t cnt{}; data[prop.get_name().to_string().c_str()].contains(std::to_string(cnt)); ++cnt)
                            {
                                val.emplace_back(PE::vec2{ data[prop.get_name().to_string().c_str()][std::to_string(cnt)]["x"].get<float>() , data[prop.get_name().to_string().c_str()][std::to_string(cnt)]["y"].get<float>() });
                            }
                            prop.set_value(inst, val);
                        }
                        else if (prop.get_type().get_name() == "classstd::map<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>,structstd::less<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>,classstd::allocator<structstd::pair<classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>const,classstd::basic_string<char,structstd::char_traits<char>,classstd::allocator<char>>>> >")
                        {
                            std::map<std::string, std::string> val;
                            if (data.contains(prop.get_name().to_string().c_str()))
                            {
                                val = data[prop.get_name().to_string().c_str()].get<std::map<std::string, std::string>>();
                            }
                            prop.set_value(inst, val);
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool SerializationManager::LoadAudioComponent(const size_t& r_id, const nlohmann::json& r_json)
{
    if (r_json["Entity"]["components"].contains("AudioComponent"))
    {
        PE::AudioComponent audioComponent;

        audioComponent.SetAudioKey(r_json["Entity"]["components"]["AudioComponent"]["audioKey"].get<std::string>());
        audioComponent.SetLoop(r_json["Entity"]["components"]["AudioComponent"]["loop"].get<bool>());

        PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::AudioComponent>(), static_cast<void*>(&audioComponent));

        // Load resource
        PE::ResourceManager::GetInstance().AddAudioKeyToLoad(audioComponent.GetAudioKey());
        return true;
    }
    return false;
}

bool SerializationManager::LoadGUISlider(const EntityID& r_id, const nlohmann::json& r_json)
{
    if (r_json["Entity"]["components"].contains("GUISlider"))
    {
        PE::GUISlider slider;

        const auto& sliderJson = r_json["Entity"]["components"]["GUISlider"];
        if (sliderJson.contains("m_Hovered")) slider.m_Hovered = sliderJson["m_Hovered"].get<bool>();
        if (sliderJson.contains("m_disabled")) slider.m_disabled = sliderJson["m_disabled"].get<bool>();
        if (sliderJson.contains("m_clicked")) slider.m_clicked = sliderJson["m_clicked"].get<bool>();
        if (sliderJson.contains("m_startPoint")) slider.m_startPoint = sliderJson["m_startPoint"].get<float>();
        if (sliderJson.contains("m_endPoint")) slider.m_endPoint = sliderJson["m_endPoint"].get<float>();
        if (sliderJson.contains("m_currentValue")) slider.m_currentValue = sliderJson["m_currentValue"].get<float>();
        if (sliderJson.contains("m_minValue")) slider.m_minValue = sliderJson["m_minValue"].get<float>();
        if (sliderJson.contains("m_maxValue")) slider.m_maxValue = sliderJson["m_maxValue"].get<float>();

        // Deserialize color vectors
        if (sliderJson.contains("m_defaultColor"))
        {
            auto& col = sliderJson["m_defaultColor"];
            slider.m_defaultColor = { col[0], col[1], col[2], col[3] };
        }
        if (sliderJson.contains("m_hoveredColor"))
        {
            auto& col = sliderJson["m_hoveredColor"];
            slider.m_hoveredColor = { col[0], col[1], col[2], col[3] };
        }
        if (sliderJson.contains("m_pressedColor"))
        {
            auto& col = sliderJson["m_pressedColor"];
            slider.m_pressedColor = { col[0], col[1], col[2], col[3] };
        }
        if (sliderJson.contains("m_disabledColor"))
        {
            auto& col = sliderJson["m_disabledColor"];
            slider.m_disabledColor = { col[0], col[1], col[2], col[3] };
        }

        // Deserialize string textures
        if (sliderJson.contains("m_defaultTexture")) slider.m_defaultTexture = sliderJson["m_defaultTexture"].get<std::string>();
        if (sliderJson.contains("m_hoveredTexture")) slider.m_hoveredTexture = sliderJson["m_hoveredTexture"].get<std::string>();
        if (sliderJson.contains("m_pressedTexture")) slider.m_pressedTexture = sliderJson["m_pressedTexture"].get<std::string>();
        if (sliderJson.contains("m_disabledTexture")) slider.m_disabledTexture = sliderJson["m_disabledTexture"].get<std::string>();

        if (sliderJson.contains("m_knobID"))
        {
            if (sliderJson["m_knobID"].is_null())
            {
                slider.m_knobID.reset();
            }
            else
            {
                slider.m_knobID = sliderJson["m_knobID"].get<EntityID>();
            }
        }

        PE::EntityFactory::GetInstance().LoadComponent(r_id, PE::EntityManager::GetInstance().GetComponentID<PE::GUISlider>(), static_cast<void*>(&slider));
        return true;
    }
    return false;
}
