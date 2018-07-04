#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

bool file_exists(const fs::path& p);

int main(int argc, char *argv[])
{

	if (!argv[1])
	{
		std::cout << "Missing directory. Remember to pass it in as a parameter." << std::endl;
		return -1;
	}

	std::cout << std::endl << "-- Sorter v1 by Joel Rodiel-Lucero--" << std::endl << std::endl;

	std::string mainDir = argv[1];

	std::vector<std::string> firstBatch;

	if (file_exists(mainDir)) {
		for (auto& p: fs::directory_iterator(mainDir))
		{
			for (auto& t: fs::directory_iterator(p))
			{
				std::ostringstream oss;

				oss << t;

				std::string file = oss.str();

				file.erase(std::remove(file.begin(), file.end(), '"'), file.end());

				firstBatch.push_back(file);
			}
		}
	}

	fs::create_directory("output");
 
	std::vector<std::string> allLines;

	for (int i = 0; i < firstBatch.size(); i++)
	{

		std::string command = "gzip -d ";

		command.append(firstBatch[i]);

		std::size_t isFlows = command.find("flows");
		std::size_t isTLS = command.find("TLS");

		if (isFlows < command.size() || isTLS < command.size())
		{

			std::cout << "Found: " << firstBatch[i] << std::endl;
			
			int suffixNum;

			if (command.substr(command.length()-3) == ".gz")
			{
				std::cout << "Starting extraction of " << firstBatch[i] << std::endl;
				
				const char* charCmmd = command.c_str();

				system(charCmmd);

				std::cout << "Extracted " << firstBatch[i] << std::endl;

				suffixNum = 11;
			}
			else
			{
				std::cout << firstBatch[i] << " is already extracted!" << std::endl;

				suffixNum = 0;
			}

			std::fstream file;

			int endOfFileName = command.length() - suffixNum;

			std::cout << "->" << command.substr(8,endOfFileName) << std::endl;

			std::string filePath = command.substr(8,endOfFileName);

			std::cout << "Opening " << filePath << " for reading..." << std::endl;

			file.open(filePath,std::ifstream::in);

			std::vector<std::string> lines;

			while (file)
			{
				std::string fileLine;

				getline(file, fileLine);

				if (isFlows < command.size())
					if (fileLine.substr(0,13) == "passive_table" && fileLine.find("Removed App ()") > fileLine.length())
						lines.push_back(fileLine);

				if (isTLS < command.size())
					lines.push_back(fileLine);
			}			

			std::cout << "Finished reading " << filePath << "!" << std::endl;

			file.close();

			bool isEmpty = false;

			if (lines.size() == 0)
				isEmpty = true;
			
			if (!isEmpty)
			{
			
				std::string name;

				if (isFlows < command.size())
					name = "output/flows_";
				else if(isTLS < command.size())
					name = "output/tls_";

				std::string mainDirStr(mainDir);

				filePath = filePath.substr(mainDirStr.size()+1);

				std::size_t endOfD = filePath.find("/");

				name.append(filePath.substr(0,endOfD));

				name.append(".csv");

				std::ofstream outfile(name);

				std::cout << "Created " << name	<< std::endl;

				std::cout << "Writing to " << name << std::endl;

				std::vector<std::string> writeLines;

				for (int j = 0; j < lines.size(); j++)
				{
					std::istringstream ss(lines[j]);

					int num = 0;

					while (ss)
					{
						num++;

						std::string lineSS;

						if (!getline( ss, lineSS, ',' ))
							break;

						if (num == 5 || num == 7 || num == 8 || num == 10 || num == 16 || num == 24 || num == 26)
							writeLines.push_back(lineSS);
					}
					
					if (isFlows < command.size())
						writeLines.push_back("NEWLINE");
				}

				if (isFlows < command.size())
					outfile << "app_package,proto_ip,dst_ip,dst_port,fqdn,app_version,app_name,client_hello" << std::endl;

				for (int j = 0; j < writeLines.size(); j++)
				{
					if (writeLines[j] != "NEWLINE")
					{
						if (writeLines[j+1] == "NEWLINE")
						{
							outfile << writeLines[j];
							allLines.push_back(writeLines[j]);
						}
						else
						{
							writeLines[j].append(",");
							outfile << writeLines[j];
							allLines.push_back(writeLines[j]);
						}
					}
					else
					{
						outfile << ",null" << std::endl;
						allLines.push_back(",null\n");
					}
				}

				outfile.close();

				std::cout << "Saved and closed " << name << std::endl;
			}
			else
			{
				std::cout << "Couldnt find any data. Skipped." << std::endl;
			}

			std::cout << std::endl;
		}
	}

	std::cout << "Created output/Applications.csv" << std::endl;

	std::ofstream finalFile("output/Applications.csv");

	finalFile << "app_package,proto_ip,dst_ip,dst_port,fqdn,app_version,app_name,client_hello" << std::endl;

	for (int i = 0; i < allLines.size(); i++)
		finalFile << allLines[i];

	std::cout << "Wrote all lines to Applications.csv" << std::endl;

	finalFile.close();

	std::cout << std::endl << "Program finished successfully! All files outputed to directory output/" << std::endl;

	return 0;
}

bool file_exists(const fs::path& p)
{
	fs::file_status s = fs::file_status{};

	if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
		return true;
	
	return false;
}
