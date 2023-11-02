using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

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
        public static Vector3 Add(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static float Dot(Vector3 a, Vector3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }
    };

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void NativeLogVector(Vector3 parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static int NativeSumIntArray(int[] numbers);
    }

    public class Entity
    {
        public float FloatVar { get; set; }

        public Entity()
        {
            PrintFormattedMessage("Entity constructor!");

            // Example usage of NativeLog
            InternalCalls.NativeLog("Entity created with message: ", 1234);

            // Example usage of NativeLogVector
            Vector3 position = new Vector3(5, 2, 1);
            InternalCalls.NativeLogVector(position);

            // Example usage of NativeSumIntArray
            int[] numbers = { 1, 2, 3, 4, 5 };
            int sum = InternalCalls.NativeSumIntArray(numbers);
            PrintFormattedMessage($"Sum of array: {sum}");
        }

        private void PrintFormattedMessage(string message)
        {
            // Set console colors
            Console.ForegroundColor = ConsoleColor.Yellow; // Text color
            Console.BackgroundColor = ConsoleColor.Black; // Background color changed to black

            // Print the message with top and bottom borders
            Console.WriteLine(new string('-', message.Length + 4));
            Console.WriteLine($"| {message} |");
            Console.WriteLine(new string('-', message.Length + 4));

            // Reset console colors to their defaults
            Console.ResetColor();
        }

        public void PrintMessage()
        {
            PrintFormattedMessage("C# Hello World");
        }

        public void PrintInt(int value)
        {
            PrintFormattedMessage($"C# output: {value}");
        }

        public void PrintInts(int value1, int value2)
        {
            PrintFormattedMessage($"C# output: {value1} and {value2}");
        }

        public void PrintCustomMessage(string message)
        {
            PrintFormattedMessage($"C# says: {message}");
        }

        // Method to demonstrate array manipulation
        public void SumArray(int[] numbers)
        {
            var sum = 0;
            foreach (var number in numbers)
            {
                sum += number;
            }
            PrintFormattedMessage($"Sum of array: {sum}");
        }

        public void PrintVectorAddition(Vector3 vec1, Vector3 vec2)
        {
            Vector3 result = Vector3.Add(vec1, vec2);
            PrintFormattedMessage($"Vector Addition: ({result.X}, {result.Y}, {result.Z})");
        }

        public void PrintDotProduct(Vector3 vec1, Vector3 vec2)
        {
            float result = Vector3.Dot(vec1, vec2);
            PrintFormattedMessage($"Dot Product: {result}");
        }
    }
}
