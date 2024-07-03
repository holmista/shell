# WISH - Wisconsin Shell

WISH (Wisconsin Shell) is a simple Unix shell implementation that provides basic command-line interface functionality. This project demonstrates fundamental concepts of process creation, management, and Unix system programming.

## Features

- Interactive and batch mode operation
- Execution of external commands
- Built-in commands: `exit`, `cd`, and `path`
- I/O redirection (> operator)
- Parallel command execution (& operator)
- Custom path management

## Compilation

To compile the WISH shell, use the following command:
```make wish```

This will create an executable named `wish`.

## Usage

### Interactive Mode

To run WISH in interactive mode, simply execute the compiled program:
```./wish```

You will see a prompt `wish> ` where you can enter commands.

### Batch Mode

To run WISH in batch mode, provide a file containing commands as an argument:
```./wish batch.txt```

## Built-in Commands

- `exit`: Exits the shell
- `cd <directory>`: Changes the current working directory
- `path <directory> [<directory> ...]`: Sets the search path for executables

## I/O Redirection

Use the `>` operator to redirect output to a file:
```ls > output.txt```

## Parallel Commands

Use the `&` operator to run commands in parallel:
```ls & pwd```

## Notes

- The initial search path is set to `/bin`
- Error messages are standardized and output to stderr
- The shell supports basic whitespace handling

## Limitations

- Does not support input redirection
- Cannot use absolute or relative paths for executables
- Limited error handling for external command failures