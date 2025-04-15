#ifndef FLOW_LOG_TAGGER_H
#define FLOW_LOG_TAGGER_H

#include <string>
#include <unordered_map>

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

inline std::string trim_ctrl_m(std::string tag) {
    std::string clean = tag;
    clean.erase(std::remove(clean.begin(), clean.end(), '\r'), clean.end());
    return clean;
}

inline std::string to_uppercase(std::string input) {

    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return input;
}

#endif
