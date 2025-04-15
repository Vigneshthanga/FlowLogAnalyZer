#include "flowlog_tagger.h"
#include<iostream>
#include<fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <string>
#include <algorithm>
#include <cctype>
#include <stdexcept>

std::unordered_map<uint32_t, std::string> load_protocol_map(const std::string& filename) {
    std::unordered_map<uint32_t, std::string> result_map;
    std::ifstream file(filename);
    std::string line;
    uint32_t line_count = 0;

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return result_map;
    }

    while (std::getline(file, line)) {
        if (line_count++ == 0) continue;  // skip header
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 2) continue;

		try {
        	uint32_t proto_number = std::stoul(tokens[0]);
        	std::string proto_name = to_uppercase(tokens[1]);
        	result_map[proto_number] = proto_name;
		}

		catch (const std::exception& e) { 
			std::cerr << "Caught an error: " << e.what() << std::endl;
			continue;
		}
    }

    return result_map;
}


std::unordered_map<PortProtocolPair, std::string> load_port_protocol_tags(const std::string& filename) {
    std::unordered_map<PortProtocolPair, std::string> result_map;
    std::ifstream file(filename);
    std::string line;
    uint32_t line_count = 0;

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return result_map;
    }

    while (std::getline(file, line)) {
        if (line_count++ == 0) continue;  // skip header
        std::stringstream ss(line);
        std::string token, proto, tag;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 3) continue;

		uint32_t port = 0;
		try {
        	port = std::stoul(tokens[0]);
        	proto = to_uppercase(trim_ctrl_m(tokens[1]));
        	tag = tokens[2];
		}

		catch (const std::exception& e) {
            std::cerr << "Caught an error: " << e.what() << std::endl;
            continue;
        }

        PortProtocolPair pair(port, proto);
        result_map[pair] = tag;
    }

    return result_map;
}

std::unordered_map<PortProtocolPair, unsigned long long> count_port_proto(const std::string& filename, std::string port_type, 
																std::unordered_map<uint32_t, std::string> protocol_map) {
    std::unordered_map<PortProtocolPair, unsigned long long> result_map;
    std::ifstream file(filename);
    std::string line;
	
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return result_map;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ' ')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 8) continue;

		/*
		* srcport  -  5
		* dstport  -  6
		* protocol -  7
		* packets  -  8
		*/

		try {
			uint32_t port = 0;

			if (!port_type.compare("destination")) {
        		port = std::stoul(tokens[6]);
			}
			else {
				port = std::stoul(tokens[5]);
			}
        	std::string proto = protocol_map[std::stoul(tokens[7])];

        	PortProtocolPair pair(port, proto);
        	result_map[pair] += std::stoul(tokens[8]);
		}

		catch (const std::exception& e) {
            std::cerr << "Caught an error: " << e.what() << std::endl;
            continue;
        }
    }

    return result_map;

}

void compute_tag_count(std::unordered_map<PortProtocolPair, unsigned long long> dstpp_count_map, 
						std::unordered_map<PortProtocolPair, std::string> pptag_map) {

	std::unordered_map<std::string, unsigned long long> result_map;

	namespace fs = std::filesystem;
	fs::create_directories("../output");
	std::string filename = "../output/tag_count.csv";
	std::ofstream file(filename);

	if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
	}

	file << "Tag,Count" << std::endl;

	for (const auto& pair : dstpp_count_map) {

		auto it = pptag_map.find(pair.first);
		if (it != pptag_map.end()) {
			std::string tag = trim_ctrl_m(pptag_map[pair.first]);
			result_map[tag] += pair.second;
		}
		else {
			result_map["Untagged"] += pair.second;
		}
	}

	for (const auto& pair : result_map) {
		file << pair.first << "," << pair.second << std::endl;
	}

	file.close();
}

void compute_pp_count(std::unordered_map<PortProtocolPair, unsigned long long> dstpp_count_map, 
						std::unordered_map<PortProtocolPair, unsigned long long> srcpp_count_map) {

	std::unordered_map<PortProtocolPair, unsigned long long> result_map;

	namespace fs = std::filesystem;
	//fs::create_directories("../output");
	std::string filename = "../output/port_count.csv";
	std::ofstream file(filename);

	if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
	}

	file << "Port,Protocol,Count" << std::endl;

	for (const auto& pair : dstpp_count_map) {
		result_map[pair.first] += pair.second;
	}

	for (const auto &pair : srcpp_count_map) {
		result_map[pair.first] += pair.second;
	}

	for (const auto& pair : result_map) {
		file << pair.first.getPort() << "," << pair.first.getProtocol() << "," << pair.second << std::endl;
	}

	file.close();

}

