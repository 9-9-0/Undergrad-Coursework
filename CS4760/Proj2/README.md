**Project Specifications:** [Here](Proj2_Specs.md)



## Usage:
* Run `Make` in the project's root directory.
* Navigate to **./bin** and execute `./master -f "../tests/testInputFile.txt" -t 60`
  * Replace the input file path with a file of your choice.
* Output files will be in the **out** folder.

## Caveats:
* Be sure to run `make clear` in the root directory to delete the previous run's output.
* **Do not delete** the dummyfile located in bin as it is used for shared memory key generation
* Try not to move around any files as I hard-coded a lot of the filepaths :)
