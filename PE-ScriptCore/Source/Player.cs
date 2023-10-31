using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    internal class Player
    {

        void onCreate()
        {
            Console.WriteLine("Player.OnCreate");
        }

        void onUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");

        }


    }
}
