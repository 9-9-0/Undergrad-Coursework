**Project Specifications:** [Here](Proj3_Specs.md)



## Caveats

- Slave processes do not check for their termination time within the critical section. Marginal implications, but thought it'd be worth noting.

## To Run 
1. In the project's root directory, run `make` for the non-verbose version or `make verbose` for the verbose version.
2. Navigate to the bin directory.
3. Run `./oss -t X -n Y -l ../out/foo -m Z`
  - X is max run time in real time
  - Y is max slave processes that can be running at any given moment
  - ../out/foo is your output log file.
  - Z is your logical nanosecond increment (depending on your processor, 1-1000 is ideal). Default is 1
