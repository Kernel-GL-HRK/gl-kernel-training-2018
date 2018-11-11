#!/usr/bin/env python3
# coding=utf-8

import os

file_path="/proc/to_uppercase/uppercase"

test_data="Hello "
more_data="world"

print("Writing data: " + test_data + more_data)
fd = os.open(file_path, os.O_RDWR | os.O_NOCTTY)
os.write(fd, test_data.encode())
os.write(fd, more_data.encode())
os.close(fd)

print("Reading all file at once:")
fd = os.open(file_path, os.O_RDWR | os.O_NOCTTY)
data = os.read(fd, 50)
print(data)
os.close(fd)

print("Reading file by chunks:")
fd = os.open(file_path, os.O_RDWR | os.O_NOCTTY)
data = os.read(fd, 5)
print(data)
data = os.read(fd, 8)
print(data)
os.close(fd)
