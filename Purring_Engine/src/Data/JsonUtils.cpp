/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     JsonUtils.cpp
 \date     25-10-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file contains the implementation of JsonUtils, a utility class in the 
           Purring Engine, designed for serializing and deserializing various data 
           types into/from JSON format. The class supports basic data types like int, 
           float, and string, as well as more complex types like vectors of integers.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#include "prpch.h"
#include "JsonUtils.h"
#include <iostream>

namespace PE {

    nlohmann::json JsonUtils::SerializeAny(const std::any& value) {
        nlohmann::json json;

        if (value.type() == typeid(int)) {
            json["type"] = "int";
            json["value"] = std::any_cast<int>(value);
        }
        else if (value.type() == typeid(float)) {
            json["type"] = "float";
            json["value"] = std::any_cast<float>(value);
        }
        else if (value.type() == typeid(std::string)) {
            json["type"] = "string";
            json["value"] = std::any_cast<std::string>(value);
        }
        else if (value.type() == typeid(std::vector<int>)) {
            json["type"] = "vector<int>";
            json["value"] = SerializeVector(std::any_cast<std::vector<int>>(value));
        }
        else {
            std::cerr << "Unsupported type for serialization" << std::endl;
        }

        return json;
    }

    std::any JsonUtils::DeserializeAny(const nlohmann::json& json) {
        if (!json.contains("type") || !json.contains("value")) {
            std::cerr << "Invalid JSON format for deserialization" << std::endl;
            return {};
        }

        std::string type = json["type"];
        if (type == "int") {
            return json["value"].get<int>();
        }
        else if (type == "float") {
            return json["value"].get<float>();
        }
        else if (type == "string") {
            return json["value"].get<std::string>();
        }
        else if (type == "vector<int>") {
            return DeserializeVector<int>(json["value"]);
        }
        else {
            std::cerr << "Unsupported type for deserialization" << std::endl;
            return {};
        }
    }

    template<typename T>
    nlohmann::json JsonUtils::SerializeVector(const std::vector<T>& vec) {
        return nlohmann::json(vec);
    }

    template<typename T>
    std::vector<T> JsonUtils::DeserializeVector(const nlohmann::json& json) {
        return json.get<std::vector<T>>();
    }

} // namespace PE
