#include <cassert> // Required for simple assertions
#include <iostream>

// The function we want to test
int add(int a, int b) { return a + b; }

// A simple test function to verify 'add'
void testAddition() {
  // Test Case 1: Positive numbers
  int result1 = add(2, 3);
  assert(result1 ==
         5); // If assertion fails, the program terminates with an error
  std::cout << "Test Case 1 Passed: 2 + 3 = 5" << std::endl;

  // Test Case 2: Negative numbers
  int result2 = add(-1, -5);
  assert(result2 == -6);
  std::cout << "Test Case 2 Passed: -1 + -5 = -6" << std::endl;

  // Test Case 3: Positive and negative
  int result3 = add(10, -5);
  assert(result3 == 5);
  std::cout << "Test Case 3 Passed: 10 + -5 = 5" << std::endl;
}

int main() {
  std::cout << "Starting tests..." << std::endl;
  testAddition();
  std::cout << "All tests completed successfully!" << std::endl;
  return 0;
}
