# Large Game Project
This project is created by students at Blekinge Institute of Technology (BTH). 
It is part of a course (PA2526) where the students aim to create a game. The project is developed on windows platform where most, 
if not all, contributors use Visual Studio 2019 (Community edition).

---
## The structure of the VS solution
The solution is, as of the date of writing this document, divided into five(5) projects.
The projects are: Engine, EngineTest, Game, Gtest and Sandbox.

##### Engine
The Engine project is where the game engine is developed. Rendering features and whatnot.
This project creates a static library which is linked to other projects in this solution.

##### EngineTests
The EngineTests project is used for unit tests of the engine.
Although this game is not intended to use Test driven development it is a testing ground nonetheless.
This project is staticly linked to the library that Engine generates and the library that GTest generates.

##### Game
This project is where the game itself is developed. It is staticly linked to the library that Engine creates.

##### GTest
This project is used to generate a static library of the google test framework. It is linked to the EngineTests project.

##### Sandbox
This project is a playground for testing out visual features developed in the engine.
It is staticly linked to the library that Engine creates.

---
## Setting up the project, welcome to premake!
This project uses premake5 to create any necessary files for the Visual Studio IDE.
Note that the setup only is aimed for contributors who use Visual Studio 2019 on Windows.

##### Step 1. Download the Premake5 executable
Make sure that you have premake downloaded! This [link](https://github.com/premake/premake-core/releases/download/v5.0.0-alpha15/premake-5.0.0-alpha15-windows.zip) goes to the download of premake5.

##### Step 2. Place the Premake5 executable
Place the downloaded executable in some accessible and "easy to find" folder on your computer. For example under the C drive such that the complete path to the executable becomes "C:\\Premake\\premake5.exe"

##### Step 3. Add premake5 to the path environment variable
The title says it all. Edit the enviroment variable "path" under systemvariables. Add the path to the previously placed Premake folder (e.g. C:\\Premake\\) and save.

##### Step 4. Build a VS solution
Almost done! Make sure to clone the Github repo and edit the build.bat file after your preference. It is however ready to build a solution for Visual Studio 2019 by default.
Run the bat file and open the solution.