clear
if g++ -o solve main.cpp -lstdc++fs -fopenmp -std=c++17; then
    ./solve "(11*2)-3"
else
    echo "G++ Build Failed"
fi