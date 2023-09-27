#include "prpch.h"
#include "SerializationManager.h"
#include "ECS/EntityFactory.h"

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

std::pair<Entity, int> SerializationManager::deserializeEntity(const nlohmann::json& j)
{
    Entity entity;                              // Create an empty Entity object to store the deserialized data.
    int entityID = -1;                          // Initialize entityID with a failure code.

    // Get a reference or pointer to your EntityManager
    PE::EntityManager* entityManager = PE::g_entityManager;

    if (j.contains("Entity"))
    {
        entity.name = j["Entity"]["name"];      // Extract the entity's name from the JSON.

        // Find the last underscore in the entity's name to extract the entityID.
        size_t lastUnderscore = entity.name.find_last_of('_');
        std::string idString;

        if (lastUnderscore != std::string::npos)
        {
            idString = entity.name.substr(lastUnderscore + 1);
        }
        else
        {
            // Handle the case when there's no underscore in the name.
            idString = "1";                      // @jw, you can define some default behavior here if you want to.
        }

        // Convert the extracted ID string to an integer, or set entityID to -1 if it's empty.
        entityID = (idString.empty()) ? -1 : std::stoi(idString);

        // Loop through the data items in the JSON and deserialize them based on their types.
        EntityID id = PE::g_entityFactory->CreateEntity();
        for (const auto& t : j["Entity"].items())
        {
            for (const auto& data : j["Entity"][t.key()].items())
            {
                std::string k = data.key();
                // example inside function
                PE::Collider col;
                PE::g_entityFactory->LoadComponent(id, k.c_str(), nullptr);
                auto dat = data.value();
                std::string type = dat["type"];
                //entity.data[k] = std::any(dat["value"].get<float>());
            }
            // Deseria
        }
        //for (const auto& [key, val] : j["Entity"]["data"].items())
        //{
        //    std::string type = val["type"];

        //    // Deserialize and store data based on their types.
        //    if (type == "int")
        //    {
        //        entity.data[key] = std::any(val["value"].get<int>());
        //    }
        //    else if (type == "float")
        //    {
        //        entity.data[key] = std::any(val["value"].get<float>());
        //    }
        //    else if (type == "string")
        //    {
        //        entity.data[key] = std::any(val["value"].get<std::string>());
        //    }
        //    else if (type == "vector<int>")
        //    {
        //        entity.data[key] = std::any(val["value"].get<std::vector<int>>());
        //    }
        //    // To add similar blocks for other data types in the future..... -hans
        //}

        // Deserialize Transform component IF it exists in JSON.
        if (j["Entity"]["components"].contains("Transform")) {
            PE::Transform t = PE::Transform::FromJson(j["Entity"]["components"]["Transform"]);

            // Add Transform component to this entity
            entityManager->Assign(entityID, "Transform");

            // Get the Transform component
            PE::Transform* transform = static_cast<PE::Transform*>(entityManager->GetComponentPoolPointer("Transform")->Get(entityID));

            // Update the component data
            *transform = t;
        }
        if (j["Entity"]["components"].contains("RigidBody")) {
            PE::RigidBody rb = PE::RigidBody::FromJson(j["Entity"]["components"]["RigidBody"]); // Assuming you implement a FromJson function

            // Assign a Rigidbody component to this entity
            entityManager->Assign(entityID, "RigidBody");

            // Get the Rigidbody component
            PE::RigidBody* rigidBody = static_cast<PE::RigidBody*>(entityManager->GetComponentPoolPointer("RigidBody")->Get(entityID));

            // Update the component data
            *rigidBody = rb;
        }
        if (j["Entity"]["components"].contains("Collider")) {
            PE::Collider col = PE::Collider::FromJson(j["Entity"]["components"]["Collider"]);

            // Assign a Collider component to this entity
            entityManager->Assign(entityID, "Collider");

            // Get the Collider component
            PE::Collider* collider = static_cast<PE::Collider*>(entityManager->GetComponentPoolPointer("Collider")->Get(entityID));

            // Update the component data
            *collider = col;
        }
        if (j["Entity"]["components"].contains("Renderer")) {
            PE::Graphics::Renderer r = PE::Graphics::Renderer::FromJson(j["Entity"]["components"]["Renderer"]);

            // Assign a Renderer component to this entity
            entityManager->Assign(entityID, "Renderer");

            // Get the Renderer component
            PE::Graphics::Renderer* renderer = static_cast<PE::Graphics::Renderer*>(entityManager->GetComponentPoolPointer("Renderer")->Get(entityID));

            // Update the component data
            *renderer = r;
        }


    }

    // Return a pair containing the deserialized entity and its entityID.
    return std::make_pair(entity, entityID);
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

std::pair<Entity, int> SerializationManager::loadFromFile(const std::string& filename)
{
    // Open the specified file for reading.
    std::ifstream inFile(filename);

    if (!inFile.is_open())
    {
        // Print an error message if the file couldn't be opened for reading.
        std::cerr << "Could not open the file for reading: " << filename << std::endl;
        return std::make_pair(Entity(), -1);        // Return an empty Entity and a failure code.
    }

    nlohmann::json j;

    // Read the JSON data from the file into the 'j' JSON object.
    inFile >> j;

    // Close the file after reading.
    inFile.close();

    // Call the deserializeEntity function to convert the JSON into an Entity and entityID.
    return deserializeEntity(j);
}
