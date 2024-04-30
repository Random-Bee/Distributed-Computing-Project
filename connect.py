import os
import sys

ip_addr = ["33.28", "33.99", "33.251", "32.183"]
main = 3

def connect():
    for i in range(len(ip_addr)):
        os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} 'cd /home/ubuntu/Code/; rm -rf ./src'")
        os.system(f"scp -r ./src ubuntu@10.200.{ip_addr[i]}:/home/ubuntu/Code")
        # os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} 'cd /home/ubuntu/Code/src; make'")
        # # if i == main:
        # os.system(f"scp ./inp.txt ubuntu@10.200.{ip_addr[i]}:/home/ubuntu/Code/bin")
        # os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} 'sudo reboot'")
        
        # if i == main: 
        #     os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} 'cd /home/ubuntu/Code/bin/; ./main {i}'")
        # else:
        #     os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} 'cd /home/ubuntu/Code/bin/; ./main {i} &'")
        # os.system(f"sshpass -p 'Student1234' ssh ubuntu@10.200.{ip_addr[i]} sudo apt install -y libssl-dev")
        pass
        
        
if __name__ == "__main__":
    connect()