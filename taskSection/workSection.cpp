#include <iostream>
#include <fstream>
#include <Windows.h>
#include <iomanip>
#include <string>
#include <winnt.h>
#include <vector>

std::string replaceStr(std::vector<char>& buffer) {
	const std::string tmpStr = "<TEMPLATE>";
	std::string strTmp(buffer.begin(), buffer.end());
	size_t pos = strTmp.find(tmpStr);
	size_t nullPos = strTmp.find("\0", pos);
	std::string stroka = strTmp.substr(pos + tmpStr.length(), nullPos);
	return stroka;
}

void PrintMsg(const char* nameSec) {
	std::fstream PEfile;
	IMAGE_DOS_HEADER PEheader;
	IMAGE_NT_HEADERS PEntheaders;
	PEfile.open("../Debug/WORK_section.exe");
	if (!PEfile.is_open()) {
		std::cout << "Error: Open PEfile" << std::endl;
		exit(1);
	}
	PEfile.read((char*)&PEheader, sizeof(IMAGE_DOS_HEADER));

	PEfile.seekg(PEheader.e_lfanew);
	PEfile.read((char*)&PEntheaders, sizeof(IMAGE_DOS_HEADER));
	if (PEntheaders.Signature != 'EP') {
		std::cout << "IMAGE_NT_HEADERS signature is incorrect" << std::endl;
		exit(1);
	}
	DWORD firstSection = PEheader.e_lfanew + PEntheaders.FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(DWORD);

	PEfile.seekg(firstSection);
	if (PEfile.bad() || PEfile.fail()) {
		std::cout << "Error: search sections" << std::endl;
		return;
	}
	DWORD SearchSection = 0;
	DWORD sectionSize = 0;
	for (int i = 0; i < PEntheaders.FileHeader.NumberOfSections; i++) {
		IMAGE_SECTION_HEADER nameSection;
		PEfile.read((char*)(&nameSection), sizeof(IMAGE_SECTION_HEADER));
		if (!strcmp(nameSec, (char*)nameSection.Name)) {
			std::cout << "Search section OK" << std::endl;
			SearchSection = nameSection.PointerToRawData;
			sectionSize = nameSection.SizeOfRawData;
			break;
		}
		else {
			if (i == PEntheaders.FileHeader.NumberOfSections - 1) {
				std::cout << "Error: search section" << std::endl;
				exit(1);
			}
			continue;
		}
	}

	PEfile.seekg(SearchSection);
	std::vector<char> buffer(sectionSize);
	PEfile.read(buffer.data(), buffer.size());
	PEfile.seekg(SearchSection);
	std::string message = replaceStr(buffer);
	MessageBoxA(NULL, message.c_str(), "STR", MB_OK);
}

int main() {
	PrintMsg(".mdata");
    return 0;
}
