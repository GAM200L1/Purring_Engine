# GAM200_Pawsitive

## Repo walkthrough:

How to update local repo
- Pull as per normal
- Download libraries (vendor.zip) from onedrive ([here](https://digipen824-my.sharepoint.com/:w:/g/personal/krystal_y_digipen_edu/EYxGu6mo5XZJvxmTTSB_6MQBt8zNiXm29dOKu_Xq9dEWKg?e=hCycpZ))
    - contains all external libraries and premake executable
- Extract vendor.zip with "Extract Here"
- Run GenerateProject.bat file
- .h file to go in include folder, .cpp to be separated further in src folder(?)
- Every time a library is added, pull the premake file and redownload the vendor zip
- Run the BAT file every time you pull the repo

How to add source file or directories

Method 1 Adding through VS
    - File/directory created will be in project root directory and not the desired location
    - Drag folder to appropriate location in file explorer, VS will add it automatically

Method 2 Adding through file explorer
    - File created will be in the desired location
    - File will not be added into VS project
    - Run BAT file for premake to update VS and add the new files in


## Suppressed Warnings in this Project
warnings supressed:
C4201: nonstandard extension used : nameless struct/union
C4819: The file contains a character that cannot be represented in the current code page(949)
C4996: localtime function of standard library. The function or variable may be unsafe. 
C4505: ('FMOD_ErrorString': unreferenced function with internal linkage has been removed)

**Linker Warnings Ignored (in properties > linker > command line):**
LNK4006: symbol already defined in object; second definition ignored
LNK4098: defaultlib 'library' conflicts with use of other libs; use /NODEFAULTLIB:library
LNK4099: PDB 'filename' was not found with 'object/library' or at 'path'; linking object as if no debug info
