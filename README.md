**HKT Text Edit Engine Project**
```
Desscription : Text Edit Engine C-implemented Text Edit Engine is a smart tool for efficiently organizing and modifying text in a console setting. Our project illustrates how the core algorithms we have studied including data structures, memory management, and user interface handling.
```
 
**Key Data Structures**
```
Linked Lists: used to store draft versions and historical records.
Stacks: created the ability to undo and redo actions.
Arrays and Strings:  for keeping names and text.
Binary Tree Concepts: applied in the navigation system version.
Hash Map Concepts: used for numerical draft selection.
Sorting Algorithms: used to display the history in the order of events.
```

 
**Core Features**
**System for Undo/Redo**
```
- keeps track of document changes using two stacks.
- allows an infinite number of undo and redo actions during a session.
- saves the complete state of the document, including timestamps and content.
```

**Tracking History**
```
- records every change made to the document with timestamps.
- keeps track of the action type, like save, load, or edit.
- allows users to view the document at any time in past.
- allows history clearing with confirmation from the user.
```

**Draft Management**
```
- allows users to create multiple document drafts.
- Easily switch between different drafts.
- Every draft has a history of editing.
- When switch between drafts, undo/redo data stays.
```

**Operations on Files**
```
- allows text from external.txt files to be loaded.
- The program efficiently handles file paths in different systems.
 ```

**Technical Implementation**
```
The system is organized into separate files by functional, following a modular design structure.
Core Module: Manages UI, program flow, and startup.
Input/Output Module: Handles text editing and file operations.
Management Module: Controls history, drafts, and versions.
Header File: Defines shared data structures and function prototypes.
```


**Project File Structure**
```
├ CMakeLists.txt           # Setting up for the build
├ texteditengine.h         # header file shared with data structures
├ core.c                   # UI and core functionality
├ inoutput.c               # Text editing and file operations
├manage.c                  # Draft and history management
├main.c                    # Entry point for the program
```

**How to Run the Code**
**Prerequisites**
```
•	GCC or another C compiler
•	CMake (version 3.10+)
•	Standard C libraries
```


**Building the Project with CMake**
```
1.	Clone the repository
git clone <repository_url>
cd <project_directory>
2.	Create and enter the build directory:
mkdir build && cd build
3.	Generate build files and compile
cmake ..
cmake --build .
4.	 Run the application:
./bin/HKT_Master
```

**building Without Cmake**
1.	  On Unix/Linux/macOS:
```
            gcc -o HKT_Master main.c core.c manage.c 
```
2.	  On Windows:
```
            gcc -o HKT_Master.exe main.c core.c manage.c inoutput.c 	
 ```

**Platform Compatibility**
```
•	Cross-Platform Support: Fully functional on both Windows and Unix-based systems.
•	Terminal Clearing: Automatically clears the terminal using the appropriate command for each operating system.
•	File Path Handling: Ensures reliable file path handling across all supported platforms.
```
**Detailed Features and Usage Guide**
**Main Menu Options**

**Edit**
```
Function: Add text to the document that is currently accessible.
Usage: Select option 2.
             Enter text line by line. End with a period (.)
Note: Empty inputs are ignored.
```
Example:
**Input:**
```
1. Select option 1 (Edit)
2. Enter: This is my first document in the Text Edit Engine.
3. Enter: It supports multiple lines and formatting.
4. Enter a period (.) on a new line to finish.
```


**Expected Output:**
```
--------------------------------------------------
|                  CURRENT TEXT                  |
--------------------------------------------------
This is my first document in the Text Edit Engine.
It supports multiple lines and formatting.
--------------------------------------------------
|                 CURRENT DRAFT                  |
--------------------------------------------------
Current Draft: Draft 1
```

**Undo**
```
Function: Return the previous state before the last edit.
Usage: Select option 2.
Note: You can do more than one undo.
```
Example: Initial State:
Current Text: This is my first document in the Text Edit Engine.
It supports multiple lines and formatting.
**Input:**
```
1. Select option 2 (Undo)
```


**Expected Output:**
```
--------------------------------------------------
|                  CURRENT TEXT                  |
--------------------------------------------------
[Empty or previous state]
--------------------------------------------------
|                 CURRENT DRAFT                  |
--------------------------------------------------
Current Draft: Draft 1

--------------------------------------------------
✅ Undo successful
--------------------------------------------------
```

**Redo**
```

Function: Restore a previously undone change.
Usage: Select option 3.
```
Example:
**Input:**
```
1. Select option 3 (Redo)
```

**Expected Output:**
```
--------------------------------------------------
|                  CURRENT TEXT                  |
--------------------------------------------------
This is my first document in the Text Edit Engine.
It supports multiple lines and formatting.
--------------------------------------------------
|                 CURRENT DRAFT                  |
--------------------------------------------------
Current Draft: Draft 1

--------------------------------------------------
✅ Redo successful
--------------------------------------------------
```

**Drafts**
```

Function: Create and switch between document drafts.
Usage: Select option 5.
Note: Create a new draft and create name.
Can switch between drafts.
View draft details like creation time and content preview.
```
Example:
**Input:**
```
1. Select option 5 (Drafts)
2. Select 'n' for new draft
3. Enter: Project Outline
```


**Expected Output:**
```
-----------------------------------------------------------------------------------------------
| NUM |      DRAFT NAME      |      TIMESTAMP     |           TEXT PREVIEW       |
-----------------------------------------------------------------------------------------------
| 1   | Project Outline (current) | 2025-05-02 12:35:22 | (empty)                  |
-----------------------------------------------------------------------------------------------
| 2   | Draft 1               | 2025-05-02 12:15:55 | This is my first document in the Text...| 
-----------------------------------------------------------------------------------------------
--------------------------------------------------
✅ Created new draft: Project Outline
--------------------------------------------------
```


**Input:**
```
1. Select option 5 (Drafts)
2. Select 's' for switch draft
3. Enter: 2
```

**Expected Output:**
```
--------------------------------------------------
✅ Switched to draft: Draft 1
--------------------------------------------------
--------------------------------------------------
|                  CURRENT TEXT                  |
--------------------------------------------------
This is my first document in the Text Edit Engine.
It supports multiple lines and formatting.
--------------------------------------------------
|                 CURRENT DRAFT                  |
--------------------------------------------------
Current Draft: Draft 1
```

**History**
```
Function: View timestamped action logs.
Usage: Select option 4.
Note:
Sort history by timestamp.
Clear history (with confirmation).
```

Example 4: History View
**Input:**
```
1. Select option 4 (History)
```

**Expected Output:**
```
-----------------------------------------------------------------------------------------------
|  NUM  |      TIME                       |      ACTION           |               Text               |
-----------------------------------------------------------------------------------------------
|  1         | 2025-05-02 12:15:55 | Create Draft       | (empty)                     |
-----------------------------------------------------------------------------------------------
```

**Load .txt**
```
Function: Import text from a file(.txt).
Usage: Select option 6, enter the filename.
Note: If current draft has text, choose:
r = replace, a = append, 0 = cancel
Loads .txt file from current or build directory
Saves old content for undo
```


**Save .txt**
```
Function: Export text to a file(.txt)
Usage: Select option 7, enter the filename.
Note: Create a new file.
Overwrite existing file.
Append to an existing file.
Cancel operation.
```


**About Us**
```
Function: View project and team credits.
Usage: Select option 8.
```


**Exit**
```
Function: Close the program.
Usage: Select option 0.
Note: clears all of the memory that has been allocated.
```
