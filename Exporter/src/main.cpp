#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <filesystem>

#include "Source/basic.h"
#include "Source/timing.h"
#include "Source/resources.h"

namespace stdf = std::filesystem;
stdf::copy_options copy_settings = stdf::copy_options::recursive | stdf::copy_options::update_existing;

//#define ROBOTICS
//#define TESTING

//int roboCopy(std::ostream& out, const char* source, const char* destination, const char* options = "/MIR") {
//	std::ostringstream command;
//	command << "robocopy \"" << source << "\" \"" << destination << "\" " << options;
//	/*out << command.str().c_str() << newline;
//	return 0;*/
//	FILE* pipe = _popen(command.str().c_str(), "r");
//	if (!pipe) {
//		out << "Failed to run robocopy";
//	}
//	else {
//		char buffer[128];
//		while (fgets(buffer, 128, pipe) != NULL) {
//			out << buffer;
//		}
//	}
//	return _pclose(pipe);
//}

//// 0 -> success
//// -1 -> file could not be opened
//// -2 -> incorrect format
//// -3 -> robocopy error
//int parse(const char* instructionpath = ".//export.csv") {
//	std::ifstream reader(instructionpath);
//	if (reader.is_open()) {
//
//		std::string buffer;
//		char dlm;
//		
//		std::getline(reader, buffer);
//		if ((dlm = clearEnd(buffer)) == null) {
//			dlm = newline;
//		}
//
//		if ((buffer != "name,source,destination,options") && (buffer != "name,source,destination")) {
//			std::cout << "CSV file is not in the correct format." << newline;
//			return -2;
//		}
//
//		std::string name, source, destination, options;
//		while (std::getline(reader, buffer, dlm)) {
//			std::istringstream stream(buffer);
//			std::getline(stream, name, ',');
//			std::getline(stream, source, ',');
//			std::getline(stream, destination, ',');
//			if (std::getline(stream, options)) {
//				if (!options.compare(0, 8, "ROBOTICS")) {
//					std::string src, dest;
//					src.append("/MIR /XD \"").append(source).append("\\build*\"");
//					roboCopy(std::cout, source.c_str(), destination.c_str(), src.c_str());
//					src.clear();
//					std::cout << "{" << name << "}: completed mirroring attempt from " << source << " to " << destination << newline;
//
//					src.append(source).append("\\build\\debug");
//					dest.append(destination).append("\\build\\debug");
//					roboCopy(std::cout, src.c_str(), dest.c_str());
//					std::cout << "{" << name << "}: completed mirroring attempt from " << src << " to " << dest << newline;
//
//					src.erase(src.end() - 13, src.end());
//					src.append("\\build");
//					dest.erase(dest.end() - 13, dest.end());
//					dest.append("\\build");
//					roboCopy(std::cout, src.c_str(), dest.c_str(), "vscodeconfig.json /MIR");
//					std::cout << "{" << name << "}: completed mirroring attempt from " << src << " to " << dest << newline;
//
//				}
//				else {
//					roboCopy(std::cout, source.c_str(), destination.c_str(), options.c_str());
//					std::cout << "{" << name << "}: copied " << source << " to " << destination << newline;
//				}
//			}
//			else {
//				roboCopy(std::cout, source.c_str(), destination.c_str());
//				std::cout << "{" << name << "}: copied " << source << " to " << destination << newline;
//			}
//		}
//		return 0;
//	}
//	std::cout << "File does not exist - change argument, or create file in default location" << newline;
//	return -1;
//}

int parse2(const char* instructionpath = ".//export.csv") {
	std::ifstream reader(instructionpath);
	if (reader.is_open()) {

		//std::cout << std::string((std::istreambuf_iterator<char>(reader)), (std::istreambuf_iterator<char>())) << "\n\n";

		std::string buffer;
		char dlm;

		std::getline(reader, buffer);
		if ((dlm = clearEnd(buffer)) == null) {
			dlm = newline;
		}

		if ((buffer != "name,source,destination,options") && (buffer != "name,source,destination")) {
			std::cout << "CSV file is not in the correct format - header must begin with: \"name,source,destination\"\n";
			return -2;
		}

		std::string name, source, destination, options;
		while (std::getline(reader, buffer, dlm)) {
			std::istringstream stream(buffer);
			std::getline(stream, name, ',');
			std::getline(stream, source, ',');
			std::getline(stream, destination, ',');
			if (std::getline(stream, options)) {
				if (!options.compare(0, 8, "ROBOTICS")) {
					const std::filesystem::path src(source), dest(destination), build(src/"build");
					for (std::filesystem::directory_entry const& dir_entry : std::filesystem::directory_iterator{ src }) {
						if (dir_entry.path() == build) {
#ifdef TESTING
							std::cout << "Theoretical copy from {" << (build / "debug") << "} to {" << ((dest / "build") / "debug") << "}\n";
							std::cout << "Theoretical file copy from {" << (build / "vscodeconfig.json") << "} to {" << ((dest / "build") / "vscodeconfig.json") << "}\n";
#else
							try { stdf::copy((build / "debug"), ((dest / "build") / "debug"), copy_settings); }
							catch (std::filesystem::filesystem_error& e) { std::cout << e.what() << newline; }
							try { stdf::copy_file((build / "vscodeconfig.json"), ((dest / "build") / "vscodeconfig.json"), stdf::copy_options::update_existing); }
							catch (std::filesystem::filesystem_error& e) { std::cout << e.what() << newline; }
#endif
						}
						else if (dir_entry.path().filename() == "bin") { std::cout << "Skipped: " << dir_entry << newline; }
						else {
#ifdef TESTING
							std::cout << "Theoretical copy from {" << dir_entry << "} to {" << (dest / dir_entry.path().filename()) << "}\n";
#else
							try { stdf::copy(dir_entry, (dest / dir_entry.path().filename()), copy_settings); }
							catch (std::filesystem::filesystem_error& e) { std::cout << e.what() << newline; }
#endif
						}
					}
					std::cout << "{" << name << "}: copied \"" << source << "\" to \"" << destination << "\"\n\n";
				}
				else {
#ifdef TESTING
					std::cout << "Theoretical copy from {" << stdf::path(source) << "} to {" << stdf::path(destination) << "}\n";
#else
					try { stdf::copy(source, destination, copy_settings); }
					catch (std::filesystem::filesystem_error& e) { std::cout << e.what() << newline; }
#endif
					std::cout << "{" << name << "}: copied \"" << source << "\" to \"" << destination << "\" - Options detected but unused\n\n";
				}
			}
			else {
#ifdef TESTING
				std::cout << "Theoretical copy from {" << stdf::path(source) << "} to {" << stdf::path(destination) << "}\n";
#else
				try { stdf::copy(source, destination, copy_settings); }
				catch (std::filesystem::filesystem_error& e) { std::cout << e.what() << newline; }
#endif
				std::cout << "{" << name << "}: copied \"" << source << "\" to \"" << destination << "\"\n\n";
			}
		}
		return 0;
	}
	std::cout << "File (" << instructionpath << ") does not exist - change argument, or create file in default location" << newline;
	return -1;
}

//int parseRobotics(const char* instructionpath = ".//projects.csv") {
//	std::ifstream reader(instructionpath);
//	if (reader.is_open()) {
//		std::string buffer;
//		char dlm;
//
//		std::getline(reader, buffer);
//		if ((dlm = clearEnd(buffer)) == null) {
//			dlm = newline;
//		}
//
//		if ((buffer != "proj,psrc,pdest,opt") && (buffer != "proj,psrc,pdest")) {
//			std::cout << "CSV file is not in the correct format." << newline;
//			return -2;
//		}
//		const char* items[4] = { "/.vscode", "/.wpilib", "/src", "/vendordeps" };
//		uint dirs = (sizeof(items) / sizeof(*items));
//		std::string src, dest, name, source, destination, options;
//		while (std::getline(reader, buffer, dlm)) {
//			std::istringstream stream(buffer);
//			std::getline(stream, name, ',');
//			std::getline(stream, source, ',');
//			std::getline(stream, destination, ',');
//			if (std::getline(stream, options)) {
//				if (options.substr(0, 5) == "BASIC") {
//					roboCopy(std::cout, source.c_str(), destination.c_str());
//					std::cout << "{" << name << "}: completed mirroring attempt from " << source << " to " << destination << newline;
//				}
//				else {
//					for (uint i = 0; i < dirs; i++) {
//						src = source + items[i];
//						dest = destination + items[i];
//						roboCopy(std::cout, src.c_str(), dest.c_str(), options.c_str());
//						std::cout << "{" << name << "}: completed mirroring attempt from " << src << " to " << dest << newline;
//					}
//				}
//			}
//			else {
//				for (uint i = 0; i < dirs; i++) {
//					src = source + items[i];
//					dest = destination + items[i];
//					roboCopy(std::cout, src.c_str(), dest.c_str());
//					std::cout << "{" << name << "}: completed mirroring attempt from " << src << " to " << dest << newline;
//				}
//			}
//		}
//		return 0;
//	}
//	std::cout << "File does not exist - change argument, or create file in default location" << newline;
//	return -1;
//}

int main(int argc, char* argv[]) {	//add timer
	StopWatch runtime("Runtime");

	int ret = 0;
	if (argc > 1) {
		std::fstream reader(argv[1]);
		if (reader.is_open()) {
#ifdef ROBOTICS
			ret = parseRobotics(argv[1]);
#else
			ret = parse2(argv[1]);
#endif
		}
		else {
			std::cout << "Argument {" << argv[1] << "} is not valid or file does not exist\nUsage: ~path~/Exporter.exe PATH/TO/CSV.csv\n";
			ret = EXIT_FAILURE;
		}
	}
	else {
#ifdef ROBOTICS
		ret = parseRobotics();
#else
		ret = parse2();
#endif
	}

	runtime.end();

	std::cin.ignore();
	exit(ret);
}

//TODO: check if /.gradle is needed -> currently the largest folder