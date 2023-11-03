#include "prpch.h"
#include "ScriptGlue.h"
#include "mono/metadata/object.h"

namespace PE
{
#define PE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("PE.InternalCalls::" #Name, Name)

    void ScriptGlue::NativeLog(MonoString* string, int parameter)
    {
        char* cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        std::cout << str << "," << parameter << std::endl;
    }

    void ScriptGlue::NativeLogVector(PE::Vector3* vector)
    {
        std::cout << "Vector3: (" << vector->x << ", " << vector->y << ", " << vector->z << ")" << std::endl;
    }

    int ScriptGlue::NativeSumIntArray(MonoArray* array)
    {
        long sum = 0;
        uintptr_t len = mono_array_length(array);
        for (uintptr_t i = 0; i < len; ++i) {
            sum += mono_array_get(array, int, i);
        }
        return sum;
    }

    void ScriptGlue::RegisterFunctions()
    {
        PE_ADD_INTERNAL_CALL(NativeLog);
        PE_ADD_INTERNAL_CALL(NativeLogVector);
        PE_ADD_INTERNAL_CALL(NativeSumIntArray);
    }
}
