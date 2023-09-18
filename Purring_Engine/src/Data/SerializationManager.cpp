#include "prpch.h"

#include "SerializationManager.h"

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

nlohmann::json SerializationManager::serializeEntity(int entityID)
{
    nlohmann::json j;
    Entity& entity = entities[entityID];

    j["Entity"]["name"] = entity.name;

    for (const auto& [key, val] : entity.data) {
        if (val.type() == typeid(int)) {
            j["Entity"]["data"][key]["type"] = "int";
            j["Entity"]["data"][key]["value"] = std::any_cast<int>(val);
        }
        else if (val.type() == typeid(float)) {
            j["Entity"]["data"][key]["type"] = "float";
            j["Entity"]["data"][key]["value"] = std::any_cast<float>(val);
        }
        else if (val.type() == typeid(std::string)) {
            j["Entity"]["data"][key]["type"] = "string";
            j["Entity"]["data"][key]["value"] = std::any_cast<std::string>(val);
        }
        else if (val.type() == typeid(std::vector<int>)) {
            j["Entity"]["data"][key]["type"] = "vector<int>";
            j["Entity"]["data"][key]["value"] = std::any_cast<std::vector<int>>(val);
        }
        // handle other types
    }

    return j;
}

std::pair<Entity, int> SerializationManager::deserializeEntity(const nlohmann::json& j)
{
    Entity entity;
    int entityID = -1; // Initialize with a failure code

    if (j.contains("Entity")) {
        try {
            entity.name = j["Entity"]["name"];
            size_t lastUnderscore = entity.name.find_last_of('_');
            std::string idString;

            if (lastUnderscore != std::string::npos) {
                idString = entity.name.substr(lastUnderscore + 1);
            }
            else {
                // Handle the case when there's no underscore
                idString = ""; // or some other default behavior
            }

            std::cout << "Extracting entityID from: " << idString << std::endl; // Debug line
            entityID = (idString.empty()) ? -1 : std::stoi(idString); // -1 if idString is empty
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Invalid argument for stoi: " << entity.name << std::endl;
            return std::make_pair(Entity(), -1);
        }
        catch (const std::out_of_range& e) {
            std::cout << "Out of range for stoi: " << entity.name << std::endl;
            return std::make_pair(Entity(), -1);
        }

        for (const auto& [key, val] : j["Entity"]["data"].items())
        {
            std::string type = val["type"];
            if (type == "int") {
                entity.data[key] = std::any(val["value"].get<int>());
            }
            else if (type == "float") {
                entity.data[key] = std::any(val["value"].get<float>());
            }
            else if (type == "string") {
                entity.data[key] = std::any(val["value"].get<std::string>());
            }
            else if (type == "vector<int>") {
                entity.data[key] = std::any(val["value"].get<std::vector<int>>());
            }
            // handle other types
        }
    }
    return std::make_pair(entity, entityID);
}






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

void SerializationManager::saveToFile(const std::string& filename, int entityID) {
    nlohmann::json serializedEntity = serializeEntity(entityID);

    // Write JSON to file
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << serializedEntity.dump(4);  // The '4' argument pretty-prints the JSON with 4-space indentation
        outFile.close();
    }
    else {
        std::cerr << "Could not open the file for writing: " << filename << std::endl;
    }
}

std::pair<Entity, int> SerializationManager::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Could not open the file for reading: " << filename << std::endl;
        return std::make_pair(Entity(), -1); // Return an empty Entity and a failure code
    }

    nlohmann::json j;
    inFile >> j;
    inFile.close();

    return deserializeEntity(j);
}

