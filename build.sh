clear
if g++ -o solve main.cpp -lstdc++fs -fopenmp -std=c++17; then
    ./solve "(((((1-2)+3)/((4)*(5^6)))))"
else
    echo "G++ Build Failed"
fi