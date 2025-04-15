# FlowLogAnalyZer
FlowLogAnalyzer parses the log file and maps each log to a tag based on the data in lookup table.

## Packages to install
1. Install Python 3. https://docs.python-guide.org/starting/install3/osx/
   brew install python 
2. Install g++ 
    Apple clang version 16.0.0 (clang-1600.0.26.4)
    Target: arm64-apple-darwin23.6.0
    Thread model: posix
    InstalledDir: /Library/Developer/CommandLineTools/usr/bin
   brew install gcc --HEAD
3. Install googletest - https://formulae.brew.sh/formula/googletest
   brew install googletest

## Steps to generate sample data (Optional - as sample data is pushed to this Git repo.)
1. python3 src/generators/generate_vpc_logs.py
2. python3 src/generators/generate_unique_lookup.py

## Steps to Compile & Run
1. make test     =>    This will run the UT code against the source file. This will catch any UT bugs.
2. make          =>    This will compile and run the code for 500K VPC logs file and generate the corresponding output files in output/ dir.
3. make functest =>    This will run the code for FT data. FT verification script is not done. Manual verification done for both tag_count and port_count.

## Asymptotoic analysis
1. load_protocol_map => parses the spec line by line and for each line, it stores the protocol number and the corresponding string in Hashmap. - O(n) * (m)
2. load_port_protocol_tags => parses the lookup table file line by line and for each line, it stores the PortProtocolPair -> Tag in Hashmap. - O(n) * O(m)
3. count_port_proto => parses the vpc log file line by line and for each line, it stores the PortProtocolPair -> Count in Hashmap. - O(n) * O(m)
   assumptions -
   a. Count is sum of packets count
   b. 2 hashmaps built each one for srcport and dstport
   c.  
4. compute_tag_count - Parses all the entries in dstpp_count_map and for every entry it lookup the pptag_map and stores the cumulative sum for every tag in result_map. O(n)
5. compute_pp_count -  Parses all the entries in dstpp_count_map and stores the match count for every PortProtocolPair in result_map. Updates the result_map using the srcpp_count_map applying the same logic as for dstpp_count_map

VPC logs contains space separated data for below columns in same order: (Contains only version 2)
["version", "account-id", "interface-id", "srcaddr", "dstaddr", "srcport", "dstport", "protocol", "packets", "bytes", "start", "end", "action", "log-status"]
