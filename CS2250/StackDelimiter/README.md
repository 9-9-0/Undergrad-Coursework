# Stack Delimiter

### Project Objectives
- Implement a stack structure to delimit blocks of texts
- Read in lines from the user until user enters "DONE" in a separate line
- Each time a left delimiter ('{', '[', '()') is entered, the delimiter is to
  be pushed into the stack 
- Each time a right delimiter ('}', ']', '()') is entered, pop the stack and
  compare the delimiters
- If delimiters are mismatched, print out an error message with the entered delimiter's
  character and its position, along with the mismatched delimiter
- If the end of input is reached and the stack is not empty, pop all remaining delimiters
  and output error messages for each

### Concepts Utilized
- Stack Data Structure Implementation
- Try-Catch Error Testing

### Known Issues
- Nested delimiters (e.g. '{[()]}') are not delimited correctly
