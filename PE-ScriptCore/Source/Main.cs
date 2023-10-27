using System;

namespace PE
{
    public class Main
    {
        public float FloatVar { get; set; }
        public Main()
        {
            Console.WriteLine("Main constructor!");
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