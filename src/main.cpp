#include "Analyser.hpp"
#include <iostream>

int main(int, char**) {
	try {
		const tdw::Analyser analyser("testData/sources");
		analyser.printDependencyTree({ "testData/include", "testData/include/headers1" });
	} catch (const std::exception& exc) {
		std::cerr << exc.what() << std::endl;
		return EXIT_FAILURE;
	} catch (...) {
		std::cerr << "Unknown error occured. Please check the arguments and try again." << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
