#pragma once
#include "mono/metadata/object.h"

namespace PE
{
    struct Vector3
    {
        float x, y, z;
    };

    class ScriptGlue
    {
    private:
        // If these functions will not be called from outside, they should be private
        static void NativeLog(MonoString* string, int parameter);
        static void NativeLogVector(Vector3* vector);
        static int NativeSumIntArray(MonoArray* array);

    public:
        static void RegisterFunctions();
    };
}