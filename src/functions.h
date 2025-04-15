#pragma once
#include <string>
#include <unordered_map>
#include "flowlog_tagger.h"

std::unordered_map<uint32_t, std::string> load_protocol_map(const std::string& filename);

std::unordered_map<PortProtocolPair, std::string> load_port_protocol_tags(const std::string& filename);

std::unordered_map<PortProtocolPair, unsigned long long> count_port_proto(const std::string& filename, std::string port_type,
                                                                std::unordered_map<uint32_t, std::string> protocol_map);

void compute_tag_count(std::unordered_map<PortProtocolPair, unsigned long long> dstpp_count_map,
                        std::unordered_map<PortProtocolPair, std::string> pptag_map);

void compute_pp_count(std::unordered_map<PortProtocolPair, unsigned long long> dstpp_count_map,
                        std::unordered_map<PortProtocolPair, unsigned long long> srcpp_count_map);

