filename="test_cases/1_lac_2.txt"
# g++ input_generator.cpp -o ig
# ./ig > "$filename"
# echo "Input generated"
g++ main.cpp -o main
./main < "$filename" 
g++ output_validator.cpp -o ov
./ov < "$filename"
