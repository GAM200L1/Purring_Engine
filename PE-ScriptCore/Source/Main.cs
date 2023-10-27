using System;
using System.Runtime.CompilerServices;

namespace PE
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x; 
            Y = y;
            Z = z;
        }
    };

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void NativeLogVector(ref Vector3 parameter);
    }
    public class Entity
    {
        public float FloatVar { get; set; }
        public Entity()
        {
            Console.WriteLine("Main constructor!");
            //NativeLog("cat", 2313);

            //Vector3 pos = new Vector3(5,2,1);
            //NativeLog(ref pos);
            //Console.WriteLine($"{result.X}, {result.Y}, {result.Z}");
            //Console.WriteLine("{0}", InternalCalls.NativeLog_VectorDot(ref.pos));

        }
        public void PrintMessage()
        {
            Console.WriteLine("C# Hello World");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# output: {value}");
        }

        public void PrintInts(int value1, int value2)
        {
            Console.WriteLine($"C# output: {value1} and {value2}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }


    }

}