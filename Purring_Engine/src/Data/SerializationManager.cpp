#include "prpch.h"
#include "SerializationManager.h"
#include "ECS/EntityFactory.h"
#include "Math/MathCustom.h"

/*!***********************************************************************************
 \brief     Serializes an entity identified by its entityID into a JSON object.

 \tparam T             This function does not use a template.
 \param[in] entityID   The unique identifier for the entity to be serialized.
 \return nlohmann::json A JSON object containing the serialized entity data.
*************************************************************************************/
nlohmann::json SerializationManager::SerializeEntity(int entityId)
{
    PE::EntityManager* entityManager = &PE::EntityManager::GetInstance();

    EntityID eID = static_cast<EntityID>(entityId);

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

    {
        PE::Transform* transform = static_cast<PE::Transform*>(entityManager->GetComponentPoolPointer("Transform")->Get(eID));

        nlohmann::json jTransform = transform->ToJson();

        j["Entity"]["components"]["Transform"] = jTransform;
    }
    {
        PE::RigidBody* rigidBody = static_cast<PE::RigidBody*>(entityManager->GetComponentPoolPointer("RigidBody")->Get(eID));
        if (rigidBody != nullptr)
        {
            nlohmann::json jRigidBody = rigidBody->ToJson(); // Make sure your RigidBody class has a ToJson function
            j["Entity"]["components"]["RigidBody"] = jRigidBody;
        }
    }
    {
        PE::Collider* collider = static_cast<PE::Collider*>(entityManager->GetComponentPoolPointer("Collider")->Get(eID));
        if (collider != nullptr) {
            nlohmann::json jCollider = collider->ToJson();
            j["Entity"]["components"]["Collider"] = jCollider;
        }
    }
    {
        PE::Graphics::Renderer* renderer = static_cast<PE::Graphics::Renderer*>(entityManager->GetComponentPoolPointer("Renderer")->Get(eID));
        if (renderer != nullptr) {
            nlohmann::json jRenderer = renderer->ToJson();
            j["Entity"]["components"]["Renderer"] = jRenderer;
        }
    }

    return j; 
}



/*!***********************************************************************************
 \brief     Deserializes a JSON object into an entity, creating a new entity in the process.

 \tparam T             This function does not use a template.
 \param[in] j          A JSON object containing the serialized entity data.
 \return EntityID      The unique identifier for the newly created entity.
*************************************************************************************/
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



/*!***********************************************************************************
 \brief     Saves a serialized entity to a JSON file.

 \tparam T                This function does not use a template.
 \param[in] filename      The name of the file where the serialized entity will be saved.
 \param[in] entityID      The unique identifier for the entity to be serialized and saved.
 \return void             This function does not return a value but writes to a file.
*************************************************************************************/
void SerializationManager::SaveToFile(const std::string& r_filename, int entityId)
{
    nlohmann::json serializedEntity = SerializeEntity(entityId);

    std::ofstream outFile(r_filename);

    if (outFile.is_open())
    {
        outFile << serializedEntity.dump(4);

        outFile.close();
    }
    else
    {
        std::cerr << "Could not open the file for writing: " << r_filename << std::endl;
    }
}



/*!***********************************************************************************
 \brief     Loads an entity from a JSON file and deserializes it.

 \tparam T                This function does not use a template.
 \param[in] filename      The name of the file from which the serialized entity will be loaded.
 \return EntityID         Returns the unique identifier of the deserialized entity, or MAXSIZE_T if file could not be opened.
*************************************************************************************/
size_t SerializationManager::LoadFromFile(const std::string& r_filename)
{
    std::ifstream inFile(r_filename);

    if (!inFile.is_open())
    {
        std::cerr << "Could not open the file for reading: " << r_filename << std::endl;
        return MAXSIZE_T;       
    }

    nlohmann::json j;

    inFile >> j;

    inFile.close();

    return DeserializeEntity(j);
}



/*!***********************************************************************************
 \brief     Initializes the component loaders for various types of components.

 \tparam T                This function does not use a template.
 \return void             This function does not return a value but populates the component loader map.
*************************************************************************************/
void SerializationManager::LoadLoaders()
{
    m_initializeComponent.emplace("RigidBody", &SerializationManager::LoadRigidBody);
    m_initializeComponent.emplace("Collider", &SerializationManager::LoadCollider);
    m_initializeComponent.emplace("Transform", &SerializationManager::LoadTransform);
    m_initializeComponent.emplace("Renderer", &SerializationManager::LoadRenderer);
}



/*!***********************************************************************************
 \brief     Loads and initializes a RigidBody component for an entity from a JSON object.

 \tparam T                 This function does not use a template.
 \param[in] r_id           The unique identifier for the entity for which the RigidBody will be loaded.
 \param[in] r_json         The JSON object containing the serialized RigidBody data.
 \return bool              Returns true if the RigidBody component is successfully loaded, otherwise false.
*************************************************************************************/
bool SerializationManager::LoadRigidBody(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::RigidBody rb;
    rb.SetMass(r_json["Entity"]["components"]["RigidBody"]["mass"].get<float>());
    rb.SetType(static_cast<EnumRigidBodyType>(r_json["Entity"]["components"]["RigidBody"]["type"].get<int>()));
    PE::EntityFactory::GetInstance().LoadComponent(r_id, "RigidBody", static_cast<void*>(&rb));
    return true;
}



/*!***********************************************************************************
 \brief     Loads the Collider component for an entity based on the provided JSON data.

 \tparam T                This function does not use a template.
 \param[in] r_id          The unique identifier of the entity for which to load the Collider component.
 \param[in] r_json        The JSON object containing the serialized Collider data for the entity.
 \return bool             Returns true upon successful loading and initialization of the Collider component.
*************************************************************************************/
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
    PE::EntityFactory::GetInstance().LoadComponent(r_id, "Collider", static_cast<void*>(&col));
    return true;
}



/*!***********************************************************************************
 \brief     Loads the Transform component for an entity based on the provided JSON data.

 \tparam T                This function does not use a template.
 \param[in] r_id          The unique identifier of the entity for which to load the Transform component.
 \param[in] r_json        The JSON object containing the serialized Transform data for the entity.
 \return bool             Returns true upon successful loading and initialization of the Transform component.
*************************************************************************************/
bool SerializationManager::LoadTransform(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Transform trans;
    trans.height = r_json["Entity"]["components"]["Transform"]["height"].get<float>();
    trans.width = r_json["Entity"]["components"]["Transform"]["width"].get<float>();
    trans.orientation = r_json["Entity"]["components"]["Transform"]["orientation"].get<float>();
    trans.position = PE::vec2{ r_json["Entity"]["components"]["Transform"]["position"]["x"].get<float>(), r_json["Entity"]["components"]["Transform"]["position"]["y"].get<float>() };
    PE::EntityFactory::GetInstance().LoadComponent(r_id, "Transform", static_cast<void*>(&trans));
    return true;
}



/*!***********************************************************************************
 \brief     Loads the Renderer component for an entity based on the provided JSON data.

 \tparam T                This function does not use a template.
 \param[in] r_id          The unique identifier of the entity for which to load the Renderer component.
 \param[in] r_json        The JSON object containing the serialized Renderer data for the entity.
 \return bool             Returns true upon successful loading and initialization of the Renderer component.
*************************************************************************************/
bool SerializationManager::LoadRenderer(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Graphics::Renderer ren;
    ren.SetColor({ r_json["Entity"]["components"]["Renderer"]["Color"]["r"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["g"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["b"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["a"].get<float>() });
    ren.SetTextureKey(r_json["Entity"]["components"]["Renderer"]["TextureKey"].get<std::string>());
    PE::EntityFactory::GetInstance().LoadComponent(r_id, "Renderer", static_cast<void*>(&ren));
    return true;
}