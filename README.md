# ls_implement
My implementation on the ls command for my System Programming class. There were three parts to this program:
 
#### Part I:
Display the name and size of everything of any given directory path

#### Part II:
List the info for the directories/files in the given path by passing the _-l_ command, I also added the _-a_ command to display the hidden files, and display the symbolic links. All this and Part I.

#### Part III:
Recursively go through sub-directories by passing the _-R_ command, and show the number of directories and files in the end. All that and Part II.

## Commands:
Usage: [-l] [-a] [-R] [file directory]

-l, --list	: Display information

-a, --all	: Display hidden files

-R, --recursive	: Go through sub-directories recursively

-h, --help	: Display passable commands

## Utilization:
User can pass any of the given commands to display the directory path. User can pass a directory path or the program will default to the current directory.

## Examples:
```
 $ ./cl -lR ~/Documents

 $ ./cl ~/Documents -al

 $ ./cl -Ral
```
