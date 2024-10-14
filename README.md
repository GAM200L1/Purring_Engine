# Purring Engine by Team Pawsitive
<p align="center">
  <img alt="Pawsitive Logo" src="https://github.com/user-attachments/assets/172c8bdc-5be1-4758-88aa-d9dc65cb6fd1" style="max-width:50px;height:auto;"/>
</p>

`Purring_Engine` is a simple but effective custom 2D game engine, developed using C++ with OpenGL for rendering.
The engine includes:
- A level editor
- Component-based architecture
- Sprite batching and instancing
- Custom algorithms for physics and collision detection
</br>

This engine was designed to support the development of _**March of the Meows**_, a 2D turn-based strategy game with a real-time twist. More details about our game can be [found below](#March-of-the-Meows).


## Screenshots and Demos

### Instanced Rendering
<p align="center">
  <img src="https://github.com/user-attachments/assets/df7e2e1b-b94e-4243-96f9-e058bdfc364a"/>
</p>

### Particle Effects
<p align="center">
  <img src="https://github.com/user-attachments/assets/310e6036-e8f1-4279-9e29-056c92daadd3"/>
</p>

### Physics
`Purring_Engine` incorporates advanced physics capabilities to ensure realistic interactions within game environments as showcased in our brief demonstration video highlighting collision detection and object dynamics below.

https://github.com/user-attachments/assets/d2cab0f5-fba3-4da1-b3ff-7c4516e02248

</br>

The screenshot below displays the physics configuration window in the `Purring_Engine` that illustrates the collision layer matrix and other settings.
<p align="center">
  <img src="https://github.com/user-attachments/assets/c871c821-6dd2-4e83-8577-7e7f400b6298"/>
</p>

## Setting it Up
Follow these steps to get the Purring Engine up and running:
1. Unzip the `vendor.zip` file directly into the root folder by selecting "Extract Here".
2. Double-click the `GenerateProject.bat` file to run it.
3. Open the `Purring_Engine.sln` file with Microsoft Visual Studio 2022.

After completing these steps, your file directory should look like this:

![File Directory Structure](https://github.com/user-attachments/assets/f51675f4-76d0-4c1e-970c-a5ed2f78585b)


## Running the Engine
1. Ensure that the target platform in your project settings is set to "Release".
   <p align="center">
     <img src="https://github.com/user-attachments/assets/bfb1899d-4750-4322-8169-fe7bca357af2"/>
   </p>
2. Compile and build the solution by pressing (`Ctrl+Shift+B`).
3. Run the engine by pressing (`Ctrl+F5`).
4. Congratulations! You're now running `Purring_Engine`.

   <p align="center">
     <img src="https://github.com/user-attachments/assets/2d7d97f8-050d-4e46-a2e0-43681f543806"/>
   </p>
   
_For more detailed instructions and troubleshooting, please refer to the "editorGuide.pdf" in the root folder._

---

### Dependencies
`Purring_Engine` utilizes several libraries and frameworks to enhance its functionality. Below is a list of key dependencies:

- [GLFW](https://github.com/glfw/glfw)
- [GLEW](https://github.com/nigels-com/glew)
- [glm](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb)
- [imgui](https://github.com/ocornut/imgui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [nlohmann](https://github.com/nlohmann/json)
- [FreeType](https://freetype.org/)
- [RTTR](https://github.com/rttrorg/rttr)
- [FMOD](https://fmod.com/)
- [FMOD Studio](https://fmod.com/)

### Target Specs
- **Operating System**: Windows 10 (64-bit)
- **Memory**: 4 GB of RAM (8 GB or more recommended for optimal performance)
- **Graphics**: NVIDIA GeForce GTX 970M or equivalent (support for OpenGL 4.5 or later)

### Development Environment
- **IDE**: Microsoft Visual Studio 2022 (C++ Compiler)
- **SDK Version**: Windows SDK Version 10.0.x or later
- **Platform Toolset**: v143 (default for Visual Studio 2022)

</br></br>

# March of the Meows
![Title Screen](https://github.com/user-attachments/assets/30294970-b48a-4bf2-9b1a-4a828922bf80) </br>
_**March of the Meows**_ is a **turn-based strategy game with a real-time twist**!  

In this adventure, you play as Sir Meowsalot, the last defender of the small village of Meowsville. Rescue felines as you journey through the world. Then, harness the power of friendship and teamwork to defeat the nefarious Rat King.

> 🎮 **Download the game**:  
> You can [**download March of the Meows**](itch.io link) or build it with our engine by following the instructions below.

### Running the Game "March of the Meows"
1. Follow the instructions in the [**Setting it Up**](#setting-it-up) section.
2. Set the target platform to `"GameRelease"`.
3. Build the solution by pressing `Ctrl + Shift + B`.
4. Run the game by pressing `Ctrl + F5`.
5. Enjoy playing _March of the Meows_!

| Game Start! | Planning Phase | Action Time! |
|---|---|---|
 |![Screenshot 3](https://github.com/user-attachments/assets/1d68cf98-8469-4071-bca7-1895018855c1) | ![Screenshot 2](https://github.com/user-attachments/assets/4ca31055-f075-4c4f-8e79-4d751085be16) | ![Screenshot](https://github.com/user-attachments/assets/ab4b94bb-ce9c-488d-899d-d56c79d15d55) |


</br></br>

# Contributors 
**Tech Lead**

HO Brandon Jun Jie (RTIS) </br>
Role: Technical Lead, Programmer </br>
Responsibilities: Engine (Assets Management), Engine (Architecture), Librarian, Animation Editor </br>

**Programmers**

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

**Design Leads**

LAI Kar Lonng (UXGD) </br>
Role: Design Lead, Art Lead </br>
Responsibilities: Level Design, Mechanics, Concepts, Game Music, SFX, Voiceovers </br>

**Product Manager**

LEONG Aixian (UXGD) </br>
Role: Product Manager, Designer </br>
Responsibilities: Story, Systems Design, Production </br>
