#include "prpch.h"
#include "SerializationManager.h"

/*-----------------------------------------------------------------------------
/// <summary>
/// Deserialize a JSON object into an entity's data.

/// </summary>
/// <param name="j">The JSON object containing the serialized entity data.</param>
/// <param name="entityId">Reference to the entity ID that will be updated with the deserialized value.</param>
/// <param name="sm">Reference to the SerializationManager used for deserialization.</param>
----------------------------------------------------------------------------- */
void PlayerStats::from_json(const nlohmann::json& j, int& entityId, SerializationManager& sm)
{
    entityId = j.at("entityId").get<int>();                                // Deserialize entityId from JSON

    sm.setEntityName(entityId, j.at("entityName").get<std::string>());     // Deserialize entityName from JSON

    // Deserialize data from JSON
    health = j.at("health").get<int>();
    level = j.at("level").get<int>();   
    experience = j.at("experience").get<float>();
    playerName = j.at("playerName").get<std::string>();
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Serialize an entity's data into a JSON object.
/// This function takes an entity ID as input and serializes the entity's data
/// into a JSON format, including its name and various data fields.
/// </summary>
/// <param name="entityId">The ID of the entity to serialize.</param>
/// <param name="sm">Reference to the SerializationManager for additional data.</param>
/// <returns>A JSON object containing the serialized entity data.</returns>
----------------------------------------------------------------------------- */
nlohmann::json PlayerStats::to_json(int entityId, SerializationManager& sm) const
{
    nlohmann::json j;                               // Create a JSON object

    j["entityId"] = entityId;                       // Serialize entityId to JSON
    j["entityName"] = sm.getEntityName(entityId);   // Serialize entityName to JSON

    // serialize thhe data from JSON
    j["health"] = health;
    j["level"] = level;
    j["experience"] = experience;
    j["playerName"] = playerName;
    return j;                                       // Return the JSON object
}



/*-----------------------------------------------------------------------------
/// <summary>
/// Serialize an entity's data into a JSON object.
/// This function takes an entity ID as input and serializes the entity's data
/// into a JSON format, including its name and various data fields.
/// </summary>
/// <param name="entityID">The ID of the entity to serialize.</param>
/// <returns>A JSON object containing the serialized entity data.</returns>
----------------------------------------------------------------------------- */
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
        for (const auto& [key, val] : j["Entity"]["data"].items())
        {
            std::string type = val["type"];

            // Deserialize and store data based on their types.
            if (type == "int")
            {
                entity.data[key] = std::any(val["value"].get<int>());
            }
            else if (type == "float")
            {
                entity.data[key] = std::any(val["value"].get<float>());
            }
            else if (type == "string")
            {
                entity.data[key] = std::any(val["value"].get<std::string>());
            }
            else if (type == "vector<int>")
            {
                entity.data[key] = std::any(val["value"].get<std::vector<int>>());
            }
            // To add similar blocks for other data types in the future..... -hans
        }

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






/*-----------------------------------------------------------------------------
/// <summary>
/// Save a serialized Entity to a JSON file.
/// This function takes a serialized Entity in JSON format and writes it to a
/// specified file. It includes the entity's data and structure.
/// </summary>
/// <param name="filename">The name of the file to save the JSON data to.</param>
/// <param name="entityID">The ID of the entity to serialize and save.</param>
----------------------------------------------------------------------------- */
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



/*-----------------------------------------------------------------------------
/// <summary>
/// Load and deserialize an Entity from a JSON file.
/// This function reads a JSON file containing serialized entity data and
/// deserializes it into an Entity object. It returns a pair containing the
/// deserialized Entity and an associated entity ID.
/// </summary>
/// <param name="filename">The name of the JSON file to load data from.</param>
/// <returns>A pair containing the deserialized Entity and its associated entity ID,
///          or an empty Entity and a failure code if the file cannot be opened.</returns>
----------------------------------------------------------------------------- */
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


/*------------------------------------------------ OLD STUFF -----------------------------------------------------*/


//nlohmann::json SerializationManager::serializeEntity(int entityID)
//{
//    nlohmann::json j;
//
//    // Serializing Entity
//    j["Entity"]["id"] = entities[entityID].id;
//    j["Entity"]["someInt"] = entities[entityID].someInt;
//    j["Entity"]["someFloat"] = entities[entityID].someFloat;
//    j["Entity"]["someDouble"] = entities[entityID].someDouble;
//    j["Entity"]["someChar"] = std::string(1, entities[entityID].someChar);  // Convert char to string for JSON
//    j["Entity"]["someBool"] = entities[entityID].someBool;
//    j["Entity"]["someString"] = entities[entityID].someString;
//
//    return j;
//}

//std::pair<Entity, int> SerializationManager::deserializeEntity(const nlohmann::json& j)
//{
//    Entity e;
//    e.id = j["Entity"]["id"];
//    e.someInt = j["Entity"]["someInt"];
//    e.someFloat = j["Entity"]["someFloat"];
//    e.someDouble = j["Entity"]["someDouble"];
//    e.someChar = j["Entity"]["someChar"].get<std::string>()[0];  // Convert string to char
//    e.someBool = j["Entity"]["someBool"];
//    e.someString = j["Entity"]["someString"];
//
//    return { e, e.id };
//}

//void SerializationManager::saveToFile(const std::string& filename, int entityID)
//{
//    std::ofstream output_file(filename);
//    if (output_file.is_open())
//    {
//        std::cout << "Direct entity ID from entities map: " << entities[5].id << std::endl;
//
//        nlohmann::json j = serializeEntity(entityID);
//        std::cout << "Saving entity with ID: " << entityID << " to file: " << filename << std::endl;
//
//        // Use dump(4) for pretty-printing with 4-space indentation
//        output_file << j.dump(4);
//        output_file.close();
//    }
//    else
//    {
//        // Error handling
//        std::cerr << "Failed to open the file for writing: " << filename << std::endl;
//    }
//}
//
//
//std::pair<Entity, int> SerializationManager::loadFromFile(const std::string& filename)
//{
//    std::ifstream input_file(filename);
//    nlohmann::json j;
//    if (input_file.is_open())
//    {
//        input_file >> j;
//        input_file.close();
//        return deserializeEntity(j);
//    }
//    else
//    {
//        // Error handlin
//        std::cerr << "Failed to open the file for reading: " << filename << std::endl;
//        return { Entity{}, -1 };                            // Return an 'invalid' entity and ID
//    }
//}