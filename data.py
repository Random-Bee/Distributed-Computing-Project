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

if __name__ == "__main__":
    write_data()
    print("Data written to file")