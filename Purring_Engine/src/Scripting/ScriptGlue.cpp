#include "prpch.h"
#include "ScriptGlue.h"
#include "mono/metadata/object.h"

namespace PE
{
#define PE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("PE.InternalCalls::" #Name, Name)

    static void NativeLog(MonoString* string, int parameter)
    {
        char* cStr = mono_string_to_utf8(string);
        std::string str(cStr);
        mono_free(cStr);
        std::cout << str << "," << parameter << std::endl;
    }

    //static void NativeLogVector(glm::vec3* parameter)
    //{
    //    std::cout << str << "," << parameter << std::endl;
    //}

	void ScriptGlue::RegisterFunctions()
	{
        PE_ADD_INTERNAL_CALL(NativeLog);
        //PE_ADD_INTERNAL_CALL(NativeLogVector);

		//mono_add_internal_call("PE.InternalCalls::NativeLog", NativeLog);
		//mono_add_internal_call("PE.InternalCalls::NativeLogVector", NativeLogVector);
	}
}
