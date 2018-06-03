Usage Instructions:
  1. Run 'make' within the Proj1 directory.
  2. The executable will output within the bin/ subdirectory.
  3. Run 'bin/scanner [file]' within the Proj1 directory with your desired arguments (integers or a single input file).
      An example with a pre-existing test file would be 'bin/scanner tests/P1_test3.txt'.
  4. Run 'make clean' to clean up object and binary files.

Caveats:
  The scanner is set up as an object. The testing function is named testScanner() as written in the specs.
  Within main.cpp, a scanner is initialized, parsed, and then testScanner() is called, which calls fetch_token() until all tokens
  have been extracted. The tokens are then returned and printed out all at once. 
