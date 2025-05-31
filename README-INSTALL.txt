Notes on Compiling and Installing wild_life++ Using MS Visual Studio C++

Visual Studio Options
Under Project
then
Under Properties
then Under command line
add
/ZCstrictstrings-
/permissive
/sdl-

Need Environment variables (set in Windows Settings)
HOME
Then life_local belongs in HOME folder
If using cygwin - requared for test suite
CYG_HOME -
Then life_local belongs in CYG_HOME folder (uSE Windows C:\Users ... notation
If running under cygwin need -cyg flag
wild_life++ -cyg
Need to copy wild_life++ from Vusual Studio wild_life++/x64/Debug folder to where it will be found (every time after any change)

Files for setup are in repository life_local
Also files for test suite.
ts.sh in Tests/bin runs the test suite
In addition to the default cygwin installation,
I installed
tsch
and
dos2unix

