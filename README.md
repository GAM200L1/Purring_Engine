![Stay Pawsitive!](https://github.com/GAM200L1/Purring_Engine/assets/93074650/0f33f7c1-61bd-43ba-ada4-c2c33717d5f6)
# March of the Meows | Pawsitive

## Guide to Updating Your Local Repository:

### Steps to Refresh Your Local Copy:
- Regularly pull updates as you would normally do using `git pull`.
- Retrieve the necessary external libraries and tools by downloading `vendor.zip` from OneDrive ([available here](https://digipen824-my.sharepoint.com/:w:/g/personal/krystal_y_digipen_edu/EYxGu6mo5XZJvxmTTSB_6MQBt8zNiXm29dOKu_Xq9dEWKg?e=hCycpZ)). This archive includes:
    - All required external libraries
    - The premake executable for project setup
- Unzip `vendor.zip` directly into the current directory using the "Extract Here" option.
- Execute the `GenerateProject.bat` script to configure project files.
- Place header files (`.h`) in the `include` directory and separate C++ source files (`.cpp`) into the `src` directory as needed.
- Whenever a new library is integrated, ensure to fetch the latest `premake` script and re-download `vendor.zip`.
- Always run the `GenerateProject.bat` script after pulling changes from the repository to keep your environment up to date.

### Adding New Source Files or Directories:

#### Method 1: Via Visual Studio
- Creating a file or directory directly in Visual Studio will place it in the project's root directory, which might not be where you want it.
- Manually move the file or directory to the desired location using the File Explorer. Visual Studio will recognize the change and update the project structure accordingly.

#### Method 2: Using File Explorer
- Create the file in the desired location directly through the File Explorer for precise placement.
- This newly created file won't automatically be recognized by the Visual Studio project.
- To include the new file in the project, run the `GenerateProject.bat` script so that premake can update the Visual Studio project files accordingly.

## Ignored Warnings in This Project

### Suppressed Compiler Warnings:
- `C4201`: Use of non-standard extension: unnamed struct/union.
- `C4819`: Presence of a character in the file that cannot be represented in the current code page (949).
- `C4996`: Use of potentially unsafe function or variable, specifically the `localtime` function from the standard library.
- `C4505`: Removal of unreferenced function with internal linkage, such as 'FMOD_ErrorString'.

### Linker Warnings Overlooked (Configured in Properties > Linker > Command Line):
- `LNK4006`: Ignoring second definition of a symbol that's already defined in an object.
- `LNK4098`: Conflict between 'library' defaultlib and other libraries; resolved by using `/NODEFAULTLIB:library`.
- `LNK4099`: Missing PDB 'filename' for 'object/library' or at specified 'path'; linking the object as if no debug info were available.

## Last Updated
This document was last updated on 17 March 2024 by Hans.
