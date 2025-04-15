#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "../functions.h"
#include <gtest/gtest.h>

void create_test_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

TEST(LoadProtocolMapTest, ParsesValidCSV) {

    std::string filename = "test_protocols.csv";
    std::string csv_content = "protocol_number,protocol_name\n"
                              "6,TCP\n"
                              "17,udp\n"
                              "1,icmp\n";
    
    create_test_file(filename, csv_content);

    auto result = load_protocol_map(filename);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[6], "TCP");
    EXPECT_EQ(result[17], "UDP");
    EXPECT_EQ(result[1], "ICMP");

    std::remove(filename.c_str());

}

TEST(LoadProtocolMapTest, HandlesInvalidLines) {
    std::string filename = "test_protocols_invalid.csv";
    std::string csv_content = "protocol_number,protocol_name\n"
                              "garbage,data\n"
                              "8\n"
                              "2,IGMP\n";
    
    create_test_file(filename, csv_content);

    auto result = load_protocol_map(filename);

    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[2], "IGMP");

    std::remove(filename.c_str());
}

TEST(LoadProtocolMapTest, HandlesMissingFile) {
    std::string filename = "non_existent.csv";
    auto result = load_protocol_map(filename);
    EXPECT_TRUE(result.empty());
}

TEST(LoadPortProtocolTagsTest, ParsesValidCSV) {
    std::string filename = "test_port_proto.csv";
    std::ofstream file(filename);
    file << "Port,Protocol,Tag\n";
    file << "80,TCP,Web\n";
    file << "53,UDP,DNS\r\n";  // Simulating ctrl-M with carriage return

    file.close();

    auto result = load_port_protocol_tags(filename);

    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(trim_ctrl_m(result[PortProtocolPair(80, "TCP")]), "Web");
    EXPECT_EQ(trim_ctrl_m(result[PortProtocolPair(53, "UDP")]), "DNS");

    std::remove(filename.c_str());
}

TEST(CountPortProtoTest, ParsesFileCorrectly) {
    // Create a mock input file
    std::string filename = "test_flowlog.txt";
    std::ofstream test_file(filename);

    ASSERT_TRUE(test_file.is_open());

    /*
    Sample flowlog line format (tokens separated by space):
    token[5] = srcport
    token[6] = dstport
    token[7] = protocol number
    token[8] = packet count
    */
    test_file << "ignored ignored ignored ignored ignored "
              << "12345 80 6 100" << std::endl;  // dstport=80, proto=6 (TCP), packets=100
    test_file << "ignored ignored ignored ignored ignored "
              << "54321 443 17 200" << std::endl;  // dstport=443, proto=17 (UDP), packets=200

    test_file.close();

    // Mock protocol map
    std::unordered_map<uint32_t, std::string> protocol_map = {
        {6, "TCP"},
        {17, "UDP"}
    };

    // Call the function
    auto result = count_port_proto(filename, "destination", protocol_map);

    PortProtocolPair p1(80, "TCP");
    PortProtocolPair p2(443, "UDP");

    // Assert counts
    ASSERT_EQ(result[p1], 100);
    ASSERT_EQ(result[p2], 200);

    // Cleanup
    std::remove(filename.c_str());
}

TEST(ComputeTagCountTest, WritesCorrectCSVOutput) {
    // Mock input data
    PortProtocolPair pair1(80, "TCP");
    PortProtocolPair pair2(443, "TCP");
    PortProtocolPair pair3(53, "UDP");

    std::unordered_map<PortProtocolPair, unsigned long long> count_map = {
        {pair1, 100},
        {pair2, 200},
        {pair3, 300}
    };

    std::unordered_map<PortProtocolPair, std::string> tag_map = {
        {pair1, "WEB"},
        {pair2, "WEB"},
        {pair3, "DNS"}
    };

    // Call function
    compute_tag_count(count_map, tag_map);

    // Read and verify file content
    std::ifstream file("../output/tag_count.csv");
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();
    std::remove("../output/tag_count.csv");  // Clean up

    ASSERT_EQ(lines.size(), 3);  // Header + 2 tags

    EXPECT_EQ(lines[0], "Tag,Count");

    // Match unordered results
    std::unordered_map<std::string, std::string> content_map;
    for (size_t i = 1; i < lines.size(); ++i) {
        auto pos = lines[i].find(',');
        ASSERT_NE(pos, std::string::npos);
        std::string tag = lines[i].substr(0, pos);
        std::string count = lines[i].substr(pos + 1);
        content_map[tag] = count;
    }

    EXPECT_EQ(content_map["WEB"], "300");  // 100 + 200
    EXPECT_EQ(content_map["DNS"], "300");
}

TEST(ComputePPCountTest, MergesSourceAndDestinationCountsCorrectly) {
    // Setup: Create dummy PortProtocolPairs and maps
    PortProtocolPair pair1(80, "TCP");
    PortProtocolPair pair2(443, "TCP");
    PortProtocolPair pair3(53, "UDP");

    std::unordered_map<PortProtocolPair, unsigned long long> dstpp_count_map = {
        {pair1, 100},
        {pair2, 150}
    };

    std::unordered_map<PortProtocolPair, unsigned long long> srcpp_count_map = {
        {pair2, 50},
        {pair3, 300}
    };

    // Call the function
    compute_pp_count(dstpp_count_map, srcpp_count_map);

    // Open the output file
    std::ifstream file("../output/port_count.csv");
    ASSERT_TRUE(file.is_open());

    // Read all lines
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    std::remove("../output/port_count.csv");  // Clean up

    ASSERT_EQ(lines.size(), 4);  // Header + 3 entries
    EXPECT_EQ(lines[0], "Port,Protocol,Count");

    // Verify content in a flexible way
    std::unordered_map<std::string, std::string> count_map;
    for (size_t i = 1; i < lines.size(); ++i) {
        std::stringstream ss(lines[i]);
        std::string port, proto, count;
        std::getline(ss, port, ',');
        std::getline(ss, proto, ',');
        std::getline(ss, count, ',');
        count_map[port + "-" + proto] = count;
    }

    EXPECT_EQ(count_map["80-TCP"], "100");
    EXPECT_EQ(count_map["443-TCP"], "200");  // 150 + 50
    EXPECT_EQ(count_map["53-UDP"], "300");
}
/*
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Running unit tests..." << std::endl;
    return RUN_ALL_TESTS();
}
*/
