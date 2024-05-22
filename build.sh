clear
if g++ -o solve main.cpp -lstdc++fs -fopenmp -std=c++17; then
    ./solve "-((-5 + -12) * (2 * -0) + -(2/-1))"
else
    echo "G++ Build Failed"
fi