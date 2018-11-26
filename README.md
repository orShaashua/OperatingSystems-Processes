# OperatingSystems-Processes
In this exercise you will write a program in C language that will execute the shell. The program will display a "prompt>"
And allows the user to type commands in unix (eg sleep, cat, ls.) After pressing
ENTER, the command entered in a separate process is executed and a new prompt is displayed for a new command.
If not written otherwise, the father process will wait until the exit process before proceeding.
If at the end of the command the user entered the character '&' => the shell will ran the command in the background.

Requirements:
1) The shell user can enter any simple command in unix. There is no need to identify complex commands
which contain pipe or redirection, but you must enable entering arguments to the command.
2) After write the command and pressing ENTER, the shell will display the new process PID oon the shell.
3) The 'job' command will display the list of commands running in the background as soon as the command is run.
