#include <iostream>
#include <fstream>
#include <Windows.h>
#include <iomanip>
#include <string>
#include <winnt.h>
#include <vector>

class Parsing {
private:
	std::fstream PEfile;
	IMAGE_DOS_HEADER PEheader;
	IMAGE_NT_HEADERS PEntheaders;

public:
	Parsing(const char* nameFile) {
		this->PEfile.open("../../WORK_section/Debug/WORK_section.exe" );
		std::cout << "PE file : " << nameFile << std::endl;
	}

	void CheckPEfile() {
		if (!this->PEfile.is_open()) {
			std::cout << "Error: Open PEfile" << std::endl;
			exit(1);
		}
		this->PEfile.read((char*)&this->PEheader, sizeof(IMAGE_DOS_HEADER));

		this->PEfile.seekg(this->PEheader.e_lfanew);
		this->PEfile.read((char*)&this->PEntheaders, sizeof(IMAGE_DOS_HEADER));
		if (this->PEntheaders.Signature != 'EP') {
			std::cout << "IMAGE_NT_HEADERS signature is incorrect" << std::endl;
			exit(1);
		}
	}

	void PrintOptional() {
		std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
		std::cout << std::setw(16) << std::hex << std::showbase << std::left;
		std::cout << "Basic information:	" << std::endl;
		std::cout << "AddressOfEntryPoint:	" << this->PEntheaders.OptionalHeader.AddressOfEntryPoint << std::endl;
		std::cout << "BaseOfCode:		" << this->PEntheaders.OptionalHeader.BaseOfCode << std::endl;
		std::cout << "BaseOfData:		" << this->PEntheaders.OptionalHeader.BaseOfData << std::endl;
		std::cout << "CheckSum:		" << this->PEntheaders.OptionalHeader.CheckSum << std::endl;
		std::cout << "DataDirectory:		" << this->PEntheaders.OptionalHeader.DataDirectory << std::endl;
		std::cout << "DllCharacteristics:	" << this->PEntheaders.OptionalHeader.DllCharacteristics << std::endl;
		std::cout << "FileAlignment:		" << this->PEntheaders.OptionalHeader.FileAlignment << std::endl;
		std::cout << "ImageBase:		" << this->PEntheaders.OptionalHeader.ImageBase << std::endl;
		std::cout << "LoaderFlags:		" << this->PEntheaders.OptionalHeader.LoaderFlags << std::endl;
		std::cout << "Magic:			" << this->PEntheaders.OptionalHeader.Magic << std::endl;
		std::cout << "MajorImageVersion:	" << this->PEntheaders.OptionalHeader.MajorImageVersion << std::endl;
		std::cout << "MajorOperSysVersion:	" << this->PEntheaders.OptionalHeader.MajorOperatingSystemVersion << std::endl;
		std::cout << "MajorSubsystemVersion:  " << this->PEntheaders.OptionalHeader.MajorSubsystemVersion << std::endl;
		std::cout << "MinorImageVersion:	" << this->PEntheaders.OptionalHeader.MinorImageVersion << std::endl;
		std::cout << "MinorOperSysVersion:	" << this->PEntheaders.OptionalHeader.MinorOperatingSystemVersion << std::endl;
		std::cout << "MinorSubsysVersion:	" << this->PEntheaders.OptionalHeader.MinorSubsystemVersion << std::endl;
		std::cout << "NumberOfRvaAndSizes:	" << this->PEntheaders.OptionalHeader.NumberOfRvaAndSizes << std::endl;
		std::cout << "SectionAlignment:	" << this->PEntheaders.OptionalHeader.SectionAlignment << std::endl;
		std::cout << "SizeOfCode:		" << this->PEntheaders.OptionalHeader.SizeOfCode << std::endl;
		std::cout << "SizeOfHeaders:		" << this->PEntheaders.OptionalHeader.SizeOfHeaders << std::endl;
		std::cout << "SizeOfHeapCommit:	" << this->PEntheaders.OptionalHeader.SizeOfHeapCommit << std::endl;
		std::cout << "SizeOfImage:		" << this->PEntheaders.OptionalHeader.SizeOfImage << std::endl;
		std::cout << "SizeOfInitializedData:  " << this->PEntheaders.OptionalHeader.SizeOfInitializedData << std::endl;
		std::cout << "SizeOfStackCommit:	" << this->PEntheaders.OptionalHeader.SizeOfStackCommit << std::endl;
		std::cout << "SizeOfStackReserve:	" << this->PEntheaders.OptionalHeader.SizeOfStackReserve << std::endl;
		std::cout << "SizeOfUninitData:	" << this->PEntheaders.OptionalHeader.SizeOfUninitializedData << std::endl;
		std::cout << "Subsystem:		" << this->PEntheaders.OptionalHeader.Subsystem << std::endl;
		std::cout << "Win32VersionValue:	" << this->PEntheaders.OptionalHeader.Win32VersionValue << std::endl;
		std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
	}

	void PrintSection() {
		DWORD firstSection = PEheader.e_lfanew + PEntheaders.FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(DWORD);

		PEfile.seekg(firstSection);
		if (PEfile.bad() || PEfile.fail()) {
			std::cout << "Error: search sections" << std::endl;
			return;
		}

		std::cout << std::hex << std::showbase << std::left;
		for (int i = 0; i < PEntheaders.FileHeader.NumberOfSections; i++) {
			IMAGE_SECTION_HEADER nameSection;
			PEfile.read((char*)(&nameSection), sizeof(IMAGE_SECTION_HEADER));
			if (PEfile.bad() || PEfile.eof())
			{
				std::cout << "Error reading section header" << std::endl;
				return;
			}
			char name[9] = { 0 };
			memcpy(name, nameSection.Name, 8);
			std::cout << "*------------------------*" << std::endl << "Section: " << name << std::endl;
			std::cout << "Virtual size:" << nameSection.Misc.VirtualSize << std::endl;
			std::cout << "Raw size:" << nameSection.SizeOfRawData << std::endl;
			std::cout << "Virtual address:" << nameSection.VirtualAddress << std::endl;
			std::cout << "Raw address:" << nameSection.PointerToRawData << std::endl;
			std::cout << "Characteristics: ";
			if (nameSection.Characteristics & IMAGE_SCN_MEM_READ)
				std::cout << "R";
			if (nameSection.Characteristics & IMAGE_SCN_MEM_WRITE)
				std::cout << "W";
			if (nameSection.Characteristics & IMAGE_SCN_MEM_EXECUTE)
				std::cout << "X";
			std::cout << std::endl << std::endl;
			std::cout << "*------------------------*" << std::endl;
		}
	}

	void ChangeStringDataByTemplate(const char* nameSec, const std::string tmpStr, const std::string& Replace) {
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
		replaceStr(buffer, Replace);
		PEfile.seekg(SearchSection);
		PEfile.write(buffer.data(), buffer.size());
		std::string message;
		for (char c : buffer) {
			if (c == '\0') {
				message += ' ';
			}
			else {
				message += c;
			}
		}
		MessageBoxA(NULL, message.c_str(), "STR", MB_OK);
	}

	void replaceStr(std::vector<char>& buffer, const std::string& strReplace) {
		const std::string tmpStr = "<TEMPLATE>";
		std::string strTmp(buffer.begin(), buffer.end());
		size_t pos = strTmp.find(tmpStr);
		std::string beforetmp = strTmp.substr(0, pos + tmpStr.length());
		size_t nullPos = strTmp.find('\0', pos + tmpStr.length());
		std::string aftertmp = strTmp.substr(nullPos + 1);
		strTmp = beforetmp + strReplace + '\0' + aftertmp;
		buffer.assign(strTmp.begin(), strTmp.end());
	}

	void searchStr(std::vector<char>& buffer, const std::string& strReplace) {}

	~Parsing() {
		this->PEfile.close();
	}
};

int main() {
	Parsing P("WORK_section.exe");
	P.CheckPEfile();
	P.PrintOptional();
	P.ChangeStringDataByTemplate(".mdata", "<Template>", "Hello ckb!!!");
	return 0;
}
