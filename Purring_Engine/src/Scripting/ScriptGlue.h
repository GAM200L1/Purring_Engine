#pragma once
#include "mono/metadata/object.h"

namespace PE
{
    /*!***********************************************************************************
     \brief     Represents a three-dimensional vector.
    *************************************************************************************/
    struct Vector3
    {
        float x, y, z;
    };


    /*!***********************************************************************************
     \brief     ScriptGlue serves as a bridge for the C# scripts to invoke native C++ functions.
    *************************************************************************************/
    class ScriptGlue
    {
    private:
        /*!***********************************************************************************
         \brief     Logs a message to the native console.
         \param     string The message to log.
         \param     parameter An integer to demonstrate overloading with parameter count.
        *************************************************************************************/
        static void NativeLog(MonoString* string, int parameter);

        /*!***********************************************************************************
         \brief     Logs a Vector3 to the native console.
         \param     vector Pointer to the Vector3 to log.
        *************************************************************************************/
        static void NativeLogVector(Vector3* vector);

        /*!***********************************************************************************
         \brief     Sums the elements of an integer array.
         \param     array MonoArray containing the integers to sum.
         \return    int The sum of the array's elements.
        *************************************************************************************/
        static int NativeSumIntArray(MonoArray* array);

    public:
        /*!***********************************************************************************
         \brief     Registers all native functions to be callable from the C# environment.
        *************************************************************************************/
        static void RegisterFunctions();
    };
}