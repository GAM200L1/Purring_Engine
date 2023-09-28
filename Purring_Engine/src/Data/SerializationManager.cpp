#include "prpch.h"
#include "SerializationManager.h"
#include "ECS/EntityFactory.h"
#include "Math/MathCustom.h"

nlohmann::json SerializationManager::serializeEntity(int entityID)
{
    // Get a reference or pointer to your EntityManager
    PE::EntityManager* entityManager = PE::g_entityManager;

    // Convert your entityID to your EntityID type
    EntityID eID = static_cast<EntityID>(entityID);

    nlohmann::json j;                           // Create a JSON object to store the serialized data.
    Entity& entity = entities[entityID];        // Get a reference to the entity with the specified ID.

    // Set the entity's name in the JSON object.
    j["Entity"]["name"] = entity.name;

    // Loop through the data associated with the entity.
    for (const auto& [key, val] : entity.data)
    {
        // Check the type of the data stored in 'val' using std::type_info.
        if (val.type() == typeid(int))
        {
            // If it's an int, add type information and the int value to JSON.
            j["Entity"]["data"][key]["type"] = "int";
            j["Entity"]["data"][key]["value"] = std::any_cast<int>(val);
        }
        else if (val.type() == typeid(float))
        {
            // If it's a float, add type information and the float value to JSON.
            j["Entity"]["data"][key]["type"] = "float";
            j["Entity"]["data"][key]["value"] = std::any_cast<float>(val);
        }
        else if (val.type() == typeid(std::string))
        {
            // If it's a string, add type information and the string value to JSON.
            j["Entity"]["data"][key]["type"] = "string";
            j["Entity"]["data"][key]["value"] = std::any_cast<std::string>(val);
        }
        else if (val.type() == typeid(std::vector<int>))
        {
            // If it's a vector of int(s), add type information and the vector value to JSON.
            j["Entity"]["data"][key]["type"] = "vector<int>";
            j["Entity"]["data"][key]["value"] = std::any_cast<std::vector<int>>(val);
        }
        // To add similar blocks for other data types in the future..... -hans
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

    return j; // Return the JSON object containing the serialized entity.
}

EntityID SerializationManager::deserializeEntity(const nlohmann::json& j)
{
    Entity entity;                              // Create an empty Entity object to store the deserialized data.
    int entityID = -1;                          // Initialize entityID with a failure code.

    // Get a reference or pointer to your EntityManager
    PE::EntityManager* entityManager = PE::g_entityManager;
    if (m_initializeComponent.empty())
        LoadLoaders();

    // This is to check if "Entity" key exists in the JSON 
    EntityID id = MAXSIZE_T;
    if (j.contains("Entity"))
    {
        const auto& entityJson = j["Entity"];

        // This is to check if "name" key exists under "Entity"
        //if (entityJson.contains("name"))
        //{
        //    entity.name = entityJson["name"];
        //}
        //else
        //{
        //    // if name is missing debuggigin
        //    std::cerr << "Error: Missing 'name' key in entity JSON." << std::endl;
        //    return std::make_pair(Entity(), -1);
        //}

        // Loop through the data items in the JSON and deserialize them based on their types.
        id = PE::g_entityFactory->CreateEntity();
        for (const auto& t : j["Entity"].items())
        {
            // to change?
            if (t.key() == "name")
            {
                entity.name = entityJson["name"];
            }
            else
            {
                for (const auto& data : j["Entity"][t.key()].items())
                {
                    const std::string k = data.key();
                    std::invoke(m_initializeComponent[k], this, id, j);
                }
                // // Deserialize components
            }
        }
    }
    // Return a pair containing the deserialized entity and its entityID.
    return id;
}

void SerializationManager::saveToFile(const std::string& filename, int entityID)
{
    // Serialize the entity with the given entityID into a JSON object.
    nlohmann::json serializedEntity = serializeEntity(entityID);

    // Open the specified file for writing.
    std::ofstream outFile(filename);

    if (outFile.is_open())
    {
        // Write the serialized JSON to the file with 4-space indentation for readability.
        outFile << serializedEntity.dump(4);

        // Close the file after writing.
        outFile.close();
    }
    else
    {
        // Print an error message if the file couldn't be opened for writing.
        std::cerr << "Could not open the file for writing: " << filename << std::endl;
    }
}

EntityID SerializationManager::loadFromFile(const std::string& filename)
{
    // Open the specified file for reading.
    std::ifstream inFile(filename);

    if (!inFile.is_open())
    {
        // Print an error message if the file couldn't be opened for reading.
        std::cerr << "Could not open the file for reading: " << filename << std::endl;
        return MAXSIZE_T;        // Return an empty Entity and a failure code.
    }

    nlohmann::json j;

    // Read the JSON data from the file into the 'j' JSON object.
    inFile >> j;

    // Close the file after reading.
    inFile.close();

    // Call the deserializeEntity function to convert the JSON into an Entity and entityID.
    return deserializeEntity(j);
}


void SerializationManager::LoadLoaders()
{
    m_initializeComponent.emplace("RigidBody", &SerializationManager::LoadRigidBody);
    m_initializeComponent.emplace("Collider", &SerializationManager::LoadCollider);
    m_initializeComponent.emplace("Transform", &SerializationManager::LoadTransform);
    m_initializeComponent.emplace("Renderer", &SerializationManager::LoadRenderer);
}

bool SerializationManager::LoadRigidBody(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::RigidBody rb;
    rb.SetMass(r_json["Entity"]["components"]["RigidBody"]["mass"].get<float>());
    rb.SetType(static_cast<EnumRigidBodyType>(r_json["Entity"]["components"]["RigidBody"]["type"].get<int>()));
    PE::g_entityFactory->LoadComponent(r_id, "RigidBody", static_cast<void*>(&rb));
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
    PE::g_entityFactory->LoadComponent(r_id, "Collider", static_cast<void*>(&col));
    return true;
}

bool SerializationManager::LoadTransform(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Transform trans;
    trans.height = r_json["Entity"]["components"]["Transform"]["height"].get<float>();
    trans.width = r_json["Entity"]["components"]["Transform"]["width"].get<float>();
    trans.orientation = r_json["Entity"]["components"]["Transform"]["orientation"].get<float>();
    trans.position = PE::vec2{ r_json["Entity"]["components"]["Transform"]["position"]["x"].get<float>(), r_json["Entity"]["components"]["Transform"]["position"]["y"].get<float>() };
    PE::g_entityFactory->LoadComponent(r_id, "Transform", static_cast<void*>(&trans));
    return true;
}

bool SerializationManager::LoadRenderer(const EntityID& r_id, const nlohmann::json& r_json)
{
    PE::Graphics::Renderer ren;
    ren.SetColor({ r_json["Entity"]["components"]["Renderer"]["Color"]["r"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["g"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["b"].get<float>(), r_json["Entity"]["components"]["Renderer"]["Color"]["a"].get<float>() });
    ren.SetTextureKey(r_json["Entity"]["components"]["Renderer"]["TextureKey"].get<std::string>());
    PE::g_entityFactory->LoadComponent(r_id, "Renderer", static_cast<void*>(&ren));
    return true;
}