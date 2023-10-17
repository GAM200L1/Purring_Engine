#include "prpch.h"
//#include "ScriptingManager.h"
//
//// Constr / destr
//ScriptingManager::ScriptingManager()
//{
//    InitializeMono();
//    LoadAssembly();
//    LoadClass();
//}
//
//ScriptingManager::~ScriptingManager()
//{
//    // Cleanup
//}
//
///// <summary>
///// ////////--------------------------------------------------------------------------------
///// </summary>
//void ScriptingManager::InitializeMono()
//{
//    domain = mono_jit_init("Purring_Engine");
//}
//
//// Load the c# assembly file (its a .dll file) that contains all the classes i want in my game engine.
//void ScriptingManager::LoadAssembly()
//{
//    assembly = mono_domain_assembly_open(domain, "script_test1.dll");
//    if (!assembly)
//    {
//        // Handle error
//    }
//
//    // THIS IS NOT A LITERALY IMAGE! This is for the c++ purring engine to "look inside/image" of the c# assembly (contains the classes and stuff).
//    image = mono_assembly_get_image(assembly);
//    if (!image)
//    {
//        // Handle error
//    }
//}
//
//void ScriptingManager::LoadClass()
//{
//    // this is to replace the namespace or classes w/ actual namespace or classes in the c# script
//    myClass = mono_class_from_name(image, "Namespace", "ClassName");
//    if (!myClass)
//    {
//        // Handle error
//    }
//
//    // create an instance of the class,this is analogous to doing `new ClassName()` in C#
//    myObject = mono_object_new(domain, myClass);
//    if (!myObject)
//    {
//        // Handle error
//    }
//
//    // Run the constructor aka simialar to calling the constructor in C#
//    mono_runtime_object_init(myObject);
//}
//
//// to run the script
//void ScriptingManager::ExecuteMoveDown()
//{
//    MonoMethod* moveDownMethod = mono_class_get_method_from_name(myClass, "MoveDown", 0);
//    mono_runtime_invoke(moveDownMethod, myObject, NULL, NULL);
//}
//
