import os
import sys
import random

inp_file = "inp-data.txt"

def write_data():
    with open(inp_file, "w") as f:
        for i in range(200):
            num1 = random.randint(1, 100000)
            f.write(f"1 {num1} {0}\n")
            num2 = random.randint(1, 100000)
            f.write(f"2 {num2}\n")
        f.write("3")

if __name__ == "__main__":
    write_data()
    print("Data written to file")