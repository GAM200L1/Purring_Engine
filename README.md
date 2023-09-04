# GAM200_Pawsitive

Repo walkthrough:

How to update local repo
- Pull as per normal.
- Download libraries (vendor.zip) from onedrive.
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