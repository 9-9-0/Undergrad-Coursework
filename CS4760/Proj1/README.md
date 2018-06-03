**"Project" Specifications:** [Here](Proj1_Specs.md)

## Caveats:

- On the requirement of using perror()...for the sake of brevity, I used strerror() in conjunction with fprintf() to output
  the specified error message format instead of constructing strings to then be passed into perror(). See line 50 of simplechain.c
  A related note, doesn't seem to be an errno for invalid argument counts, so the usage string gets printed without setting errno.
  See line 46 of simplechain.c

- Static pattern rules were used in place of suffix rules in the makefile, same functionality as far as I know.

## Usage:

1) Run make in the root of the project directory.
2) Executables will be located under the /bin folder.

## To-Do's:

- Add a shell script that runs each program with arguments passed into make, outputting into out/
