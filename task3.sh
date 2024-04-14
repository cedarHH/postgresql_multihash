make
rm  -f  R.*
./create  R  20  2  "0,1:0,2:0,3:0,4:0,5:0,6"
./insert R < data3_1.txt
echo "\ninsert 0-5\n"
./stats  R
./insert R < data3_2.txt
echo "\ninsert 6\n"
./stats  R
./insert R < data3_3.txt
echo "\ninsert 7-10\n"
./stats  R
./insert R < data3_4.txt
echo "\ninsert 11\n"
./stats  R
./insert R < data3_5.txt
echo "\ninsert 12-23\n"
./stats  R