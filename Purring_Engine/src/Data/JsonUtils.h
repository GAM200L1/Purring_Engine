#pragma once

#include <Data/json.hpp>
#include <any>
#include <vector>
#include <string>
#include <typeinfo>

namespace PE {

    class JsonUtils {
    public:
        static nlohmann::json SerializeAny(const std::any& value);
        static std::any DeserializeAny(const nlohmann::json& json);

        template<typename T>
        static nlohmann::json SerializeVector(const std::vector<T>& vec);

        template<typename T>
        static std::vector<T> DeserializeVector(const nlohmann::json& json);
    };

} // namespace PE
