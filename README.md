# Purring Engine by Team Pawsitive
<img align="center" alt="Pawsitive Logo" src="https://github.com/user-attachments/assets/172c8bdc-5be1-4758-88aa-d9dc65cb6fd1" style="max-width:50px;height:auto;"/>
</br>
</br>

A simple but effective custom 2D game engine, built using C++ with OpenGL for rendering.
Our engine features a level editor, a component-based architecture, sprite batching and instancing, and custom algorithms for physics and collision detection.
</br>

This engine was designed to support the development of "March of the Meows", our turn-based strategy game with a real-time twist. More details about our game can be [found below](#March-of-the-Meows).


## Screenshots and Demos

*Instanced Rendering*
![image](https://github.com/user-attachments/assets/df7e2e1b-b94e-4243-96f9-e058bdfc364a)

*Particle Effects*
![image](https://github.com/user-attachments/assets/310e6036-e8f1-4279-9e29-056c92daadd3)

*Physics*

https://github.com/user-attachments/assets/d2cab0f5-fba3-4da1-b3ff-7c4516e02248

![image](https://github.com/user-attachments/assets/c871c821-6dd2-4e83-8577-7e7f400b6298)

## Setting it Up
1. Extract vendor.zip in root folder with "Extract Here".
2. Run GenerateProject.bat file by double-clicking it.
3. Open the Visual Studios solution "Purring_Engine.sln" using Microsoft Visual Studios 2022.

Your file directory should now look like this: </br>
![image](https://github.com/user-attachments/assets/f51675f4-76d0-4c1e-970c-a5ed2f78585b)


## Running the Engine
1. Set the target platform to "Release". </br>
![image](https://github.com/user-attachments/assets/bfb1899d-4750-4322-8169-fe7bca357af2)
2. Build the solution (Ctrl+Shift+B).
3. Run the engine (Ctrl+F5).
4. Congrats! You're running the Purring Engine.

![image](https://github.com/user-attachments/assets/2d7d97f8-050d-4e46-a2e0-43681f543806)

_For more details on using the engine, please refer to "editorGuide.pdf" in the root folder._

### Dependencies
- GLFW 
- GLEW 
- glm
- stb_image
- FMOD     
- imgui    
- ImGuizmo 
- nlohmann 
- FreeType 
- RTTR     
- FMOD Studio

### Target Specs
- Windows 10 (x64) machine capable
- 4 GB of RAM
- NVidia GeForce GTX 970M

### Visual Studios Version
- Microsoft Visual Studios 2022 (C++ Compiler)
- SDK Version (10.0…) or later
- Platform Toolset (v143)

</br></br>

# March of the Meows
March of the Meows is a turn-based strategy game with a real-time twist! 

You play as Sir Meowsalot, the last defender of the small village of Meowsville. Rescue felines as you journey through the world. Then, harness the power of friendship and teamwork to defeat the nefarious Rat King.

The game can be downloaded [here (itch.io page)](), or by building it with our engine by following these instructions:

## Running the Game "March of the Meows"
1. Follow the instructions in the ["Setting it Up"](#Setting-it-Up) section 
2. Set the target platform to "GameRelease"
3. Build the solution (Ctrl+Shift+B).
4. Run the game (Ctrl+F5).
5. Have fun with March of the Meows!

</br></br>

# Team
*Tech Lead*

HO Brandon Jun Jie (RTIS) </br>
Role: Technical Lead, Programmer </br>
Responsibilities: Engine (Assets Management), Engine (Architecture), Librarian, Animation Editor </br>

*Programmers*

FOONG Jun Wei (RTIS) </br>
Role: Programmer </br>
Responsibilities: Engine (GameObject Factory), Debugging Tools, Reflection </br>

LIEW Yeni (RTIS) </br>
Role: Programmer </br>
Responsibilities: Resource, Physics, Collision, Particles </br>

ONG You Yang (RTIS) </br>
Role: Programmer </br>
Responsibilities: Serialization, Scripting, Audio </br>

YAMIN Krystal (RTIS) </br>
Role: Programmer </br>
Responsibilities: Graphics, Rendering </br>

TAN Jarran Yan Zhi (IMGD) </br>
Role: Programmer </br>
Responsibilities: AI, Gameplay, Event, Input, Editor </br>

*Design Leads*

LAI Kar Lonng (UXGD) </br>
Role: Design Lead, Art Lead </br>
Responsibilities: Level Design, Mechanics, Concepts, Game Music, SFX, Voiceovers </br>

*Product Manager*

LEONG Aixian (UXGD) </br>
Role: Product Manager, Designer </br>
Responsibilities: Story, Systems Design, Production </br>