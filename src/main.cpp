
#include "functions.h"

int main() {
	auto protocol_map = load_protocol_map("../src/specifications/protocol-numbers-1.csv");
	auto pptag_map = load_port_protocol_tags("../src/generators/unique_random_lookup_table_100k.csv");
	auto dstpp_count_map = count_port_proto("../src/generators/vpc_sample_logs_500k.txt", "destination", protocol_map);
	auto srcpp_count_map = count_port_proto("../src/generators/vpc_sample_logs_500k.txt", "source", protocol_map);

	compute_tag_count(dstpp_count_map,pptag_map);

	compute_pp_count(dstpp_count_map, srcpp_count_map);

    return 0;
}
