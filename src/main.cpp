#include "Analyser.hpp"
#include "Utils.hpp"
#include <iostream>
#include <variant>

int main(int argc, char* argv[]) {
	
	try {
		const auto arguments = tdw::utils::readArguments(argc, argv);
		tdw::utils::assertCompliantArguments(arguments);
		
		auto argIterator = arguments.cbegin();
		const tdw::Analyser analyser(std::get<std::string>(*argIterator++));

		auto optionArguments = std::vector<tdw::Analyser::path_type>(arguments.size() - 1);
		std::transform(argIterator, arguments.cend(), optionArguments.begin(), [](const tdw::utils::argument_type& arg) {
			const auto& optionArgument = std::get<tdw::utils::option_type>(arg);
			return tdw::Analyser::path_type{ optionArgument.second };
		});
		analyser.printDependencyTree(optionArguments);
	} catch (const std::exception& exc) {
		std::cerr << std::endl << exc.what() << std::endl;
		return EXIT_FAILURE;
	} catch (...) {
		std::cerr << std::endl << "Unknown error occured. Please check the arguments and try again." << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
