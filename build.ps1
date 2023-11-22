$sourceFile = "main.c"
$outputFile = "main.exe"

# Set the paths
$includePath = "C:/Users/jokke/OneDrive/Desktop/vscode/C/SDL2/include"
$libPath = "C:/Users/jokke/OneDrive/Desktop/vscode/C/SDL2/lib"

# Compile the program
gcc -std=c17 $sourceFile -I"$includePath" -L"$libPath" -Wall -lmingw32 -lSDL2main -lSDL2 -o $outputFile
