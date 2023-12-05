# $sourceFile = "main.c"
# $outputFile = "main.exe"

# # Set the paths
# $includePath = "C:/Users/jokke/OneDrive/Desktop/vscode/C/sdl2 first game/SDL2/include"
# $libPath = "C:/Users/jokke/OneDrive/Desktop/vscode/C/sdl2 first game/SDL2/lib"

# # Compile the program
# gcc -std=c17 $sourceFile -I"$includePath" -L"$libPath" -Wall -lmingw32 -lSDL2main -lSDL2 -o $outputFile


$sourceFile = "main.c"
$outputFile = "main.exe"

# Set the paths
$includePath = "C:/Users/jokke/OneDrive/Desktop/vscode/C/sdl2 first game/SDL2/include"
$libPath = "C:/Users/jokke/OneDrive/Desktop/vscode/C/sdl2 first game/SDL2/lib"

# Compile the program
$compileCommand = "gcc -std=c17 $sourceFile -I'$includePath' -L'$libPath' -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o $outputFile"

# Execute the compile command
Invoke-Expression $compileCommand