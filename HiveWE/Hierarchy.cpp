#include "stdafx.h"

Hierarchy hierarchy;

void Hierarchy::init() {
	std::cout << "Loading CASC data from: " << warcraft_directory << "\n";
	game_data.open(warcraft_directory / "Data");
	aliases.load("filealiases.json");
}

BinaryReader Hierarchy::open_file(const fs::path& path) const {
	casc::File file;

	if (map.file_exists(path)) {
		auto file_content = map.file_open(path).read();
		return BinaryReader(file_content);
	} else if (fs::exists((warcraft_directory / "War3Mod.mpq") / path)) {
		std::ifstream fin(((warcraft_directory / "War3Mod.mpq") / path).string(), std::ios_base::binary);
		fin.seekg(0, std::ios::end);
		size_t fileSize = fin.tellg();
		fin.seekg(0, std::ios::beg);
		std::vector<uint8_t> buffer(fileSize);
		fin.read(reinterpret_cast<char*>(&buffer[0]), fileSize);
		fin.close();
		return BinaryReader(buffer);
	} else if (game_data.file_exists("war3.mpq:"s + tileset + ".mpq:"s + path.string())) {
		file = game_data.file_open("war3.mpq:"s + tileset + ".mpq:"s + path.string());
	} else if (game_data.file_exists("enus-war3local.mpq:"s + path.string())) {
		file = game_data.file_open("enus-war3local.mpq:"s + path.string());
	} else if (game_data.file_exists("war3.mpq:"s + path.string())) {
		file = game_data.file_open("war3.mpq:"s + path.string());
	} else if (game_data.file_exists("deprecated.mpq:"s + path.string())) {
		file = game_data.file_open("deprecated.mpq:"s + path.string());
	} else {
		if (aliases.exists(path.string())) {
			return open_file(aliases.alias(path.string()));
		} else {
			std::cout << "Unable to find file in hierarchy: " << path << "\n";
			return BinaryReader(std::vector<uint8_t>());
		}
	}
	return BinaryReader(file.read());
}

bool Hierarchy::file_exists(const fs::path& path) const {
	return map.file_exists(path)
		|| fs::exists((warcraft_directory / "War3Mod.mpq") / path)
		|| game_data.file_exists("war3.mpq:"s + tileset + ".mpq:"s + path.string())
		|| game_data.file_exists("enus-war3local.mpq:"s + path.string())
		|| game_data.file_exists("war3.mpq:"s + path.string())
		|| game_data.file_exists("deprecated.mpq:"s + path.string())
		|| ((aliases.exists(path.string())) ? file_exists(aliases.alias(path.string())) : false );
}