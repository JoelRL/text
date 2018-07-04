#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <experimental/filesystem>

struct termios orig_termios;

namespace fs = std::experimental::filesystem;

void enableRawMode();

bool rawMode;

void disableRawMode();

void render(std::vector<std::string>& lines);

bool file_exists(const fs::path& p);

int main(int argc, char* argv[])
{

	// Environment variables
	bool noFile = false;
	std::string filePath;
	std::vector<std::string> lines;
	std::string messageConsole;
	bool running = true;
	std::string mode = "cmd";

	// Map for actions
	std::map<std::string,int> actions;
	actions[":q"] = 1;
	actions[":w"] = 2;
	actions[":wq"] = 3;
	actions["i"] = 4;

	// If there are no arguments, no file has been passed
	if (argc == 1)
		noFile = true;

	if (noFile)
	{
		// NO FILE
		std::cout << "Status: No file.\n";
	}
	else
	{
		// Get file path
		filePath = argv[1];

		// If file exists...
		if (file_exists(filePath))
		{
			// Open file for reading
			std::fstream file;
			file.open(filePath,std::ifstream::in);

			while (file)
			{
				std::string fileLine;

				getline(file, fileLine);

				lines.push_back(fileLine);
			}

			messageConsole = "Opened " + filePath;
		}
		else
		{
			messageConsole = "Status: File doesnt exist. Creating " + filePath;

			// Create file
			std::ofstream file(filePath);

			file.close();

			if (!file_exists(filePath))
			{
				std::cout << "Error: Unable to create file!\n";
				return -1;
			}
		}

		// Do first render
		render(lines);

		// Main loop
		for (;;)
		{
		
			if (mode == "cmd")
			{	
				render(lines);
				
				std::string command;

				std::cout << std::endl << messageConsole << std::endl;

				getline(std::cin, command);

				int action = actions[command];
				
				switch (action)
				{
					case 0:
						messageConsole = "Couldnt recognize command.";
						break;
					case 1:
						messageConsole = "Exited " + filePath + "\nq";
						running = false;
						break;
					case 2:
						messageConsole = "Wrote to " + filePath;
						break;
					case 3:
						messageConsole = "Wrote to and exited from " + filePath;
						break;
					case 4:
						messageConsole = "-- INSERT --";
						mode = "insert";
						system("clear");
						enableRawMode();
						break;
				}
			}
			else if (mode == "insert")
			{
				char key;

				read(STDIN_FILENO, &key, 1);

				if (key == 'q')
					running = false;
			}

			if (!running)
				break;

		}
	}

	if (rawMode)
		disableRawMode();

	return 0;
}

void render(std::vector<std::string>& lines)
{

	system("clear");

	for (unsigned int i = 0; i < lines.size(); i++)
	{
		std::cout << lines[i] << std::endl;
	}
}

bool file_exists(const fs::path& p)
{
	fs::file_status s = fs::file_status{};

	if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
		return true;
	
	return false;
}

void enableRawMode()
{
	tcgetattr(STDIN_FILENO, &orig_termios);

	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON);
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

	rawMode = true;
}

void disableRawMode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

	rawMode = false;
}
