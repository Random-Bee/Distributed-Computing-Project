import os
import sys
import random

inp_file = "inp-data.txt"

def write_data():
    with open(inp_file, "w") as f:
        for i in range(200):
            num = random.randint(1, 100000)
            f.write(f"1 {num} {0}\n")
            f.write(f"2 {num}\n")
        f.write("3")


nodes = [4, 8, 16, 32, 64, 128, 256]
curr = "kademlia"

def run_and_collect():
    for node in nodes:
        for i in range(5):
            write_data()
            os.system(f"cp ./inp-data.txt ./bin/inp-data.txt")
            os.system(f"touch ./bin/inp.txt; echo {node} > ./bin/inp.txt;")
            os.system(f"echo '127.0.0.1' >> ./bin/inp.txt;")
            os.system(f"cd ./bin; ./main 0")
            os.system("mkdir -p data")
            os.system(f"mv ./bin/log.txt ./data/log_{curr}_{node}_{i}.txt")   
            os.system(f"mv ./bin/key.txt ./data/key_{curr}_{node}_{i}.txt")
            os.system(f"mv ./bin/time.txt ./data/time_{curr}_{node}_{i}.txt")
            
        
if __name__ == "__main__":
    run_and_collect()
    print("Done")