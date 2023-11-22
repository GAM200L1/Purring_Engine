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
