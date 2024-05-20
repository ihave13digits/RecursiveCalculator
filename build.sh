clear
if g++ -o solve main.cpp -lstdc++fs -fopenmp -std=c++17; then
    ./solve "-1--2.003+(-7-(-6*-23)+-0.01)"
else
    echo "G++ Build Failed"
fi