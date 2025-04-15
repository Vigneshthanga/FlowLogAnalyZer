
#include "functions.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <protocol_map_file> <port_protocol_tag_file> <log_file>" << std::endl;
        return 1;
    }

    std::string protocol_map_file = argv[1];
    std::string pptag_file = argv[2];
    std::string log_file = argv[3];

    auto protocol_map = load_protocol_map(protocol_map_file);
    auto pptag_map = load_port_protocol_tags(pptag_file);
    auto dstpp_count_map = count_port_proto(log_file, "destination", protocol_map);
    auto srcpp_count_map = count_port_proto(log_file, "source", protocol_map);

    compute_tag_count(dstpp_count_map, pptag_map);
    compute_pp_count(dstpp_count_map, srcpp_count_map);

    return 0;
}

