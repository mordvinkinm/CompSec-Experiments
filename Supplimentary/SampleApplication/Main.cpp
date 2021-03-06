/****************************************************************
 *
 *	Sample x86 application that emulates real application flow:
 *	- reads two integers from console
 *	- prints out their sum
 *
 ****************************************************************/

#include <iostream>

int sum(int a, int b) {
	return a + b;
}

int main()
{
	int a, b;

	std::cout << "Please enter two numbers using space as a separator" << std::endl;

	std::cin >> a >> b;
	std::cout << a << " + " << b << " = " << sum(a, b) << std::endl;

	system("pause");

	return EXIT_SUCCESS;
}
