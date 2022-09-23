#!/bin/bash
# Runs all .cpp files
./a.out
echo ""
echo "Log:"
cat log.txt
echo ""
echo "Points:"
cat points.txt
echo ""
echo "Output: "
cat output.txt
convert output.ppm output.jpeg