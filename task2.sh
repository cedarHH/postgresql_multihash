make
rm  -f  R.*
./create R 3 8 "0,0:1,0:2,0:0,1:1,1:2,1"
./insert R < data1.txt
./stats  R
echo "\n./select R '1042,?,?' | sort\n"
./select R '1042,?,?' | sort

echo "\n./select R '?,horoscope,?' | sort\n"
./select R '?,horoscope,?' | sort

echo "\n./select R '?,?,shoes' | sort\n"
./select R '?,?,shoes' | sort

echo "\n./select R '?,shoes,?' | sort\n"
./select R '?,shoes,?' | sort

echo "\n./select R '?,chair,shoes' | sort\n"
./select R '?,chair,shoes' | sort

echo "\n./select R '?,shoes,chair' | sort\n"
./select R '?,shoes,chair' | sort