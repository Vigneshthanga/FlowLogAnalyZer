import random
import csv
import ipaddress

# Constants
num_entries = 500_000
max_int32 = 2**31 - 1
max_int64 = 2**63 - 1
actions = ["ACCEPT"] * 80 + ["REJECT"] * 20
log_statuses = ["OK", "NODATA", "SKIPDATA"]
interface_ids = [f"eni-{random.randint(10000000, 99999999):08x}" for _ in range(100)]

def random_ip(version=4):
    if version == 4:
        return str(ipaddress.IPv4Address(random.randint(0, 2**32 - 1)))
    else:
        return str(ipaddress.IPv6Address(random.getrandbits(128)))

def generate_entry():
    version = "2"
    account_id = "123456789012"
    interface_id = random.choice(interface_ids)
    
    # 60% IPv4, 40% IPv6
    srcaddr = random_ip(4 if random.random() < 0.6 else 6)
    dstaddr = random_ip(4 if random.random() < 0.6 else 6)

    srcport = random.randint(0, 65535)
    dstport = random.randint(0, 65535)
    protocol = random.randint(0, 147)

    packets = random.randint(0, max_int64)
    bytes_transferred = random.randint(0, max_int64)

    start = random.randint(1600000000, 1700000000)
    end = start + random.randint(1, 300)

    action = random.choice(actions)
    log_status = random.choice(log_statuses)

    return [version, account_id, interface_id, srcaddr, dstaddr, srcport, dstport,
            protocol, packets, bytes_transferred, start, end, action, log_status]

def write_log_file(filename):
    with open(filename, "w", newline='') as file:
        writer = csv.writer(file, delimiter=' ')
        #writer.writerow(["version", "account-id", "interface-id", "srcaddr", "dstaddr",
        #                 "srcport", "dstport", "protocol", "packets", "bytes",
        #                 "start", "end", "action", "log-status"])
        for _ in range(num_entries):
            writer.writerow(generate_entry())

if __name__ == "__main__":
    write_log_file("vpc_sample_logs_500k.txt")
    print("Generated vpc_sample_logs_500k.txt with 500,000 entries")

