#!/bin/zsh

generate_datasets() {

    distributions=(brvrwr brvrwn brvrwu
                  brvnwr brvnwn brvnwu
                  brvuwr brvuwn brvuwu
                  bnvrwr bnvrwn bnvrwu
                  bnvnwr bnvnwn bnvnwu
                  bnvuwr bnvuwn bnvuwu
                  buvrwr buvrwn buvrwu   
                  buvnwr buvnwn buvnwu 
                  buvuwr buvuwn buvuwu)
    
    
    for item in "${distributions[@]}"; do
        echo "$item"

        #M
        src/bin/generate_dataset -o data/intervals/M/small/$item/small.txt   -a 2:4 -g 6:10  -i g5 -d $item
        src/bin/generate_dataset -o data/intervals/M/medium/$item/medium.txt -a 2:4 -g 11:15 -i g5 -d $item
        src/bin/generate_dataset -o data/intervals/M/big/$item/big.txt       -a 2:4 -g 16:20 -i g5 -d $item

        #N
        src/bin/generate_dataset -o data/intervals/N/small/$item/small.txt   -a 2:6   -r 1:2 -i a5 -d $item
        src/bin/generate_dataset -o data/intervals/N/medium/$item/medium.txt -a 7:11  -r 1:2 -i a5 -d $item
        src/bin/generate_dataset -o data/intervals/N/big/$item/big.txt       -a 12:16 -r 1:2 -i a5 -d $item

        #M_OVER_N
        src/bin/generate_dataset -o data/intervals/M_OVER_N/small/$item/small.txt   -a 2:3 -r 1:5 -i r5 -d $item
        src/bin/generate_dataset -o data/intervals/M_OVER_N/medium/$item/medium.txt -a 2:3 -r 1:5 -i r5 -d $item
        src/bin/generate_dataset -o data/intervals/M_OVER_N/big/$item/big.txt       -a 2:3 -r 1:5 -i r5 -d $item

        #Permutations
        src/bin/generate_dataset -o data/intervals/permutations/small/$item/small.txt  -a 2:4 -r 1:3 -p 0:100 -i p10 -d $item
        src/bin/generate_dataset -o data/intervals/permutations/medium/$item/medium.txt -a 2:4 -r 1:3 -p 0:100 -i p10 -d $item
        src/bin/generate_dataset -o data/intervals/permutations/big/$item/big.txt       -a 2:4 -r 1:3 -p 0:100 -i p10 -d $item

        #Values
        src/bin/generate_dataset -o data/intervals/values/small/$item/small.txt   -a 2:4 -r 1:3 -v 0:100 -i v10 -d $item
        src/bin/generate_dataset -o data/intervals/values/medium/$item/medium.txt -a 2:4 -r 1:3 -v 0:100 -i v10 -d $item
        src/bin/generate_dataset -o data/intervals/values/big/$item/big.txt       -a 2:4 -r 1:3 -v 0:100 -i v10 -d $item

        #Budget used percent
        src/bin/generate_dataset -o data/intervals/budget_percent/small/$item/small.txt   -a 2:4 -r 1:3 -b 50:250 -i b5 -d $item
        src/bin/generate_dataset -o data/intervals/budget_percent/medium/$item/medium.txt -a 2:4 -r 1:3 -b 50:250 -i b5 -d $item
        src/bin/generate_dataset -o data/intervals/budget_percent/big/$item/big.txt       -a 2:4 -r 1:3 -b 50:250 -i b5 -d $item

    done

    FILES=$(find data/intervals -type f | wc -l)
    echo "$FILES files was created"
}

generate_datasets
