import os
import sys

ip_addr = ["33.28", "33.99", "33.251", "32.183"]

def connect():
    for i in range(len(ip_addr)):
        os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} echo \"1\" &")
        
if __name__ == "__main__":
    connect()