Usage Instructions:
  1. Run 'make' within the Proj4 directory.
  2. The executable will output within the bin/ subdirectory.
  3. Run 'bin/comp [file]' within the Proj4 directory with the file to compile.
      An example with a pre-existing test file would be 'bin/comp tests/ifstat_test0'.
  4. The .asm file will be output in the directory that you ran the executable from (recommended to run this from Proj4 root as I haven't
     configured it much).
  5. Run 'make clean' to clean up object, binary, and outputted .asm files.

To-Do's:
  - Add in -o option for a specified output directory/file

Caveats:
  - As listed in the specs, the optimization is simply a post-processing filter that looks for the pattern:
    STORE x
    LOAD x

    and removes the unnecessary LOAD statement.
  - If the target .asm file already exists, it will be overwritten with the new output target.
  - It is assumed that the directory this code is generated in is writable.
