Usage Instructions:
  1. Run 'make' within the Proj2 directory.
  2. The executable will output within the bin/ subdirectory.
  3. Run 'bin/frontEnd [file]' within the Proj2 directory with the file to parse.
      An example with a pre-existing test file would be 'bin/frontEnd tests/test_prog4'.
  4. Run 'make clean' to clean up object and binary files.

To-Do's:
  1. Refactor grammar rules for better maintainability
  2. Refactor parser errors for better maintanability

Caveats:
  - The printing function mentioned in the specs is treated as a member function of the parser class (no testTree.cpp).
  - For non-terminals that expand into an empty set, no node is created. Formally, then, the parse tree is invalid. However,
    from a functional standpoint this lessens the workload of the backend code generation as empty set nodes will not need to be processed.
  - Syntactic keywords (e.g. start, end, iter, etc) are not stored within their respective nodes.
