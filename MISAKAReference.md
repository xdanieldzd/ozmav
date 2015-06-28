## About libMISAKA ##

libMISAKA is a console and user interface library based on PDCurses, designed to allow interfacing with generally GUI-less programs (ex. with just a single OpenGL window). In addition to a console system, featuring functionality such as a command history, scrollable message log and more, the library also provides the framework and functions for creating simple text-based user interfaces.

![http://i50.tinypic.com/16466e.png](http://i50.tinypic.com/16466e.png)

## Initialization & Basic Usage ##

libMISAKA is initialized by using the function `MSK_Init` with the string to be displayed in the libMISAKA console's title bar as the parameter.

**Prototype:** `int MSK_Init(char *);`

**Example:** `MSK_Init("libMISAKA Application");`

After the system has been initialized, a string containing all the characters that are valid for input in the console has to be provided to the library. This is done using the `MSK_SetValidCharacters` function.

**Prototype:** `void MSK_SetValidCharacters(char *);`

**Example:** `MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyz 0123456789.\\/\"-");`

To add valid commands to the console, beyond the two default commands help and exit, the function `MSK_AddCommand` is used. The parameters for the function call are the command itself, a small description of the command that is shown when using the help command and finally the function that is to be called when the command is executed. Eventual parameters entered with the command are given to the function via a string, which can then be parsed using the C standard function sscanf.

**Prototype:** `void MSK_AddCommand(char *, char *, void *);`

**Example:** `MSK_AddCommand("test", "This is a test!", TestFunction);`

printf-style printing to the console is done with the `MSK_ConsolePrint` function, however, unlike printf, one of four foreground color types can be specified for the text. Those color types are `MSK_COLORTYPE_INFO` (standard grey), `MSK_COLORTYPE_OKAY` (green), `MSK_COLORTYPE_WARNING` (orange) and `MSK_COLORTYPE_ERROR` (red).

**Prototype:** `void MSK_ConsolePrint(int, char *, ...);`

**Example:** `MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "Now testing console output...\n");`

Before exiting a program, libMISAKA should be shut down using the `MSK_Exit` function. This function doesn't take any parameters.

**Prototype:** `void MSK_Exit();`

**Example:** `MSK_Exit();`

## Example Main Function ##

The following is a simple example of how to use libMISAKA inside a project:

```
int main(int argc, char * argv[])
{
  // Initialize example counter
  float Counter = 0.0f;

  // Initialize libMISAKA
  MSK_Init("Sample libMISAKA Application");
  // Set valid characters for console input
  MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyz 0123456789.\\/\"-");
  // Add console command
  MSK_AddCommand("test", "This will execute a function.", TestFunction);
  // Print message to console
  MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "Hello, world! Meet MISAKA!\n");

  // Main loop, executed as long as libMISAKA does not signal exit or error
  while(MSK_DoEvents()) {
    // Increase example counter
    Counter += 0.0001f;
  }

  // Print counter's value to console
  MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "The counter is now at %4.2f.\n", Counter);

  // Shut down libMISAKA
  MSK_Exit();

  // Exit program
  return 0;
}
```

## Function Reference ##

(TODO: function reference, user interface creation, etc. here)