#!/usr/bin bash
for file in data/*
do

for algo in {gamma,omega,delta,fib}
do
        echo ${file}
        ./release/src/lzw ${file} smieci/${file}_${algo}.enc e -a ${algo}
        ./release/src/lzw smieci/${file}_${algo}.enc smieci/${file}_${algo}.dec d -a ${algo}
        diff ${file} smieci/${file}_${algo}.dec
        echo
done

done
