CC = g++
CFLAGS = -std=c++20 -O3

CURDIR = $(PWD)
.PHONY: setup teardown run clean execute build

setup:
		mkdir -p plots data src/bin

clean:
		$(ECHO $(CURDIR))
		rm -f $(CURDIR)/src/bin/*
		rm -f $(CURDIR)/plots/*

build:
		$(CC) $(CFLAGS) '$(CURDIR)'/src/main.cpp -o '$(CURDIR)'/src/bin/main

execute: build
		#TODO: add the data generation step here
		./src/bin/main # Run the code
		python3 src/analysis.py # Run the analysis of the generated data

run: clean setup execute teardown

teardown:
		rm -f $(CURDIR)/data/*
