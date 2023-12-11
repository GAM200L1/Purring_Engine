/*!***********************************************************************************
 \project  Purring Engine
 \module   CSD2401-A
 \file     JsonUtils.h
 \date     25-10-2023

 \author               Hans (You Yang) ONG
 \par      email:      youyang.o@digipen.edu

 \brief	   This file contains the implementation of JsonUtils, a utility class in the 
           Purring Engine, designed for serializing and deserializing various data 
           types into/from JSON format. The class supports basic data types like int, 
           float, and string, as well as more complex types like vectors of integers.

 All content (c) 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*************************************************************************************/
#pragma once

#include <Data/json.hpp>
#include <any>
#include <vector>
#include <string>
#include <typeinfo>

namespace PE {

    class JsonUtils {
    public:
        /*!***********************************************************************************
         \brief     Serializes a vector of any type to json
        *************************************************************************************/
        static nlohmann::json SerializeAny(const std::any& value);

        /*!***********************************************************************************
         \brief     Deserializes a vector of any type from json
        *************************************************************************************/
        static std::any DeserializeAny(const nlohmann::json& json);

        /*!***********************************************************************************
         \brief     Serializes a vector of any type to json
        *************************************************************************************/
        template<typename T>
        static nlohmann::json SerializeVector(const std::vector<T>& vec);

        /*!***********************************************************************************
         \brief     Deserializes a vector of any type from json
        *************************************************************************************/
        template<typename T>
        static std::vector<T> DeserializeVector(const nlohmann::json& json);
    };

} // namespace PE
