## Compile and Run Instructions

### Prerequisites

- Ensure you have  `gcc` and resource compiler installed on your system.

  
#### Steps to Compile and Run
1. **Compile the Resource file**:

   Open a terminal and navigate to the directory containing the Resource file. Run the following command to compile the Resource file:
   ```sh
   rc /fo MyApp.res MyApp.rc
   windres MyApp.res MyApp.o
2. **Compile the Project**:

   After compiling resource file, compile the program by running:
   ```sh
   gcc main.c -o main.exe MyApp.o  -lcomctl32  -lcomdlg32 -mwindows
3. **Run the Executable**:

    After compiling, execute the program by running:
   ```sh
   .\main.exe
   
