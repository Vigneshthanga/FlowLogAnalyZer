#include<iostream>
#include<fstream>
#include <sstream>
#include <cstdint>
#include <vector>

class PortProtocolPair {

	private:
		uint32_t port_;
		std::string protocol_;

	public:
		PortProtocolPair(uint32_t port, std::string protocol) : port_(port), protocol_(protocol) {}


		uint32_t getPort() const { return port_; }

		std::string getProtocol() const  { return protocol_; }

	    bool operator==(const PortProtocolPair& other) const {
        return port_ == other.port_ && protocol_ == other.protocol_;
    }

	friend struct std::hash<PortProtocolPair>;

};

namespace std {
	template <>
	struct std::hash<PortProtocolPair> {
		std::size_t operator()(const PortProtocolPair& obj) const {
			std::size_t h1 = std::hash<uint32_t>{}(obj.port_);
			std::size_t h2 = std::hash<std::string>{}(obj.protocol_);
			return h1 ^ (h2 << 1); // Combine hash values
		}
	};
}


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

        uint32_t proto_number = std::stoul(tokens[0]);
        std::string proto_name = tokens[1];
        result_map[proto_number] = proto_name;
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
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 3) continue;

        uint32_t port = std::stoul(tokens[0]);
        std::string proto = tokens[1];
        std::string tag = tokens[2];

        PortProtocolPair pair(port, proto);
        result_map[pair] = tag;
    }

    return result_map;
}

int main() {

	auto protocol_map = load_protocol_map("specifications/protocol-numbers-1.csv");
	auto pptag_map = load_port_protocol_tags("generators/unique_random_lookup_table_100k.csv");

    for (const auto& pair : pptag_map) {
        std::cout << "Key: " << pair.first.getPort() << " proto: " << pair.first.getProtocol() << ", Value: " << pair.second << std::endl;
    }
	return 0;
}
