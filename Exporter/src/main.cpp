#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <chrono>

#define ROBOTICS

constexpr char newline = '\n';
constexpr char space = ' ';
constexpr char null = '\0';

typedef unsigned int uint;
typedef unsigned long long udlong;

char clearEnd(std::string& str) {
	char ret = null;
	udlong len = str.length();
	for (uint i = 1; i < 3; i++) {
		if (str[len - i] == 13 || str[len - i] == 10) {
			ret = str[len - i];
			str.pop_back();
		}
	}
	return ret;
}

int roboCopy(std::ostream& out, const char* source, const char* destination, const char* options = "/MIR") {
	std::ostringstream command;
	command << "robocopy \"" << source << "\" \"" << destination << "\" " << options;
	//out << command.str().c_str() << newline;
	FILE* pipe = _popen(command.str().c_str(), "r");
	if (!pipe) {
		out << "Failed to run robocopy";
	}
	else {
		char buffer[128];
		while (fgets(buffer, 128, pipe) != NULL) {
			out << buffer;
		}
	}
	return _pclose(pipe);
}

// 0 -> success
// -1 -> file could not be opened
// -2 -> incorrect format
// -3 -> robocopy error
int parse(const char* instructionpath = ".//export.csv") {
	std::ifstream reader(instructionpath);
	if (reader.is_open()) {
		std::string buffer;
		char dlm;
		
		std::getline(reader, buffer);
		if ((dlm = clearEnd(buffer)) == null) {
			dlm = newline;
		}

		if ((buffer != "name,source,destination,options") && (buffer != "name,source,destination")) {
			std::cout << "CSV file is not in the correct format." << newline;
			return -2;
		}
		while (std::getline(reader, buffer, dlm)) {
			std::istringstream stream(buffer);
			std::string name, source, destination, options;
			std::getline(stream, name, ',');
			std::getline(stream, source, ',');
			std::getline(stream, destination, ',');
			if (std::getline(stream, options)) {
				roboCopy(std::cout, source.c_str(), destination.c_str(), options.c_str());
				std::cout << "{" << name << "}: copied " << source << " to " << destination << newline;
			}
			else {
				roboCopy(std::cout, source.c_str(), destination.c_str());
				std::cout << "{" << name << "}: copied " << source << " to " << destination << newline;
			}
		}
		return 0;
	}
	std::cout << "File does not exist - change argument, or create file in default location" << newline;
	return -1;
}

int parseRobotics(const char* instructionpath = ".//projects.csv") {
	std::ifstream reader(instructionpath);
	if (reader.is_open()) {
		std::string buffer;
		char dlm;

		std::getline(reader, buffer);
		if ((dlm = clearEnd(buffer)) == null) {
			dlm = newline;
		}

		if ((buffer != "proj,psrc,pdest,opt") && (buffer != "proj,psrc,pdest")) {
			std::cout << "CSV file is not in the correct format." << newline;
			return -2;
		}
		std::string items[4] = { "/.vscode", "/.wpilib", "/src", "/vendordeps" };
		uint dirs = (sizeof(items) / sizeof(*items));
		std::string src, dest;
		while (std::getline(reader, buffer, dlm)) {
			std::istringstream stream(buffer);
			std::string name, source, destination, options;
			std::getline(stream, name, ',');
			std::getline(stream, source, ',');
			std::getline(stream, destination, ',');
			if (std::getline(stream, options)) {
				if (options.substr(0, 5) == "BASIC") {
					roboCopy(std::cout, source.c_str(), destination.c_str());
					std::cout << "{" << name << "}: completed mirroring attempt from " << source << " to " << destination << newline;
				}
				else {
					for (uint i = 0; i < dirs; i++) {
						src = source + items[i];
						dest = destination + items[i];
						roboCopy(std::cout, src.c_str(), dest.c_str(), options.c_str());
						std::cout << "{" << name << "}: completed mirroring attempt from " << src << " to " << dest << newline;
					}
				}
			}
			else {
				for (uint i = 0; i < dirs; i++) {
					src = source + items[i];
					dest = destination + items[i];
					roboCopy(std::cout, src.c_str(), dest.c_str());
					std::cout << "{" << name << "}: completed mirroring attempt from " << src << " to " << dest << newline;
				}
			}
		}
		return 0;
	}
	std::cout << "File does not exist - change argument, or create file in default location" << newline;
	return -1;
}

int main(int argc, char* argv[]) {	//add timer
	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	int ret = 0;
	if (argc > 1) {
		std::fstream reader(argv[1]);
		if (reader.is_open()) {
#ifdef ROBOTICS
			ret = parseRobotics(argv[1]);
#endif
#ifndef ROBOTICS
			ret = parse(argv[1]);
#endif
		}
		else {
			std::cout << "Argument {" << argv[1] << "} is not valid or file does not exist" << newline;
			std::cout << "Usage: ~path~/Exporter.exe PATH/TO/CSV.csv" << newline;
			ret = EXIT_FAILURE;
		}
	}
	else {
#ifdef ROBOTICS
		ret = parseRobotics();
#endif
#ifndef ROBOTICS
		ret = parse();
#endif
	}

	std::chrono::duration<double> runtime = std::chrono::high_resolution_clock::now() - start;
	std::cout << "Runtime: " << runtime.count() << " seconds" << newline;

	std::cin.ignore();
	exit(ret);
}

//int main(int argc, char* argv[]) {
//	if (argc > 1) {
//		for (int i = 1; i < argc; i++) {
//			std::cout << argv[i] << newline;
//		}
//	}
//	return 0;
//}