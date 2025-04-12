import random
import csv

# Constants
num_entries = 100_000
max_int32 = 2**31 - 1
protocols = ["tcp", "udp", "icmp", "igmp", "ggp", "ipv4", "st", "tcp", "cbt", "egp", "igp", 
			  "bbn-rcc-mon", "nvp-II", "pup", "argus", "emcon", "xnet", "chaos", "udp", "mux",
			  "dcn-meas", "hmp", "prm", "xns-idp", "trunk-1", "trunk-2", "leaf-1", "leaf-2", "rdp", "irtp",
			  "iso-tp4", "netblt", "mfe-nsp"]
tag_prefix = "sv_P"
output_file = "unique_random_lookup_table_100k.csv"

def generate_unique_entries(count):
    seen = set()
    results = []

    while len(results) < count:
        dstport = random.randint(0, 147)
        protocol = random.choice(protocols)
        tag = f"{tag_prefix}{random.randint(1, 1000)}"

        entry = (dstport, protocol, tag)

        if entry not in seen:
            seen.add(entry)
            results.append(entry)

    return results

def write_to_csv(data, filename):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["dstport", "protocol", "tag"])
        writer.writerows(data)

if __name__ == "__main__":
    entries = generate_unique_entries(num_entries)
    write_to_csv(entries, output_file)
    print(f"Generated {num_entries} unique entries and saved to {output_file}")

