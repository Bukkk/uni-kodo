#!/usr/bin bash
for algo in {gamma,omega,delta,fib}
do
        ./release/src/lzw data/pan-tadeusz-czyli-ostatni-zajazd-na-litwie.txt smieci/tad_${algo}.enc e -a ${algo}
        ./release/src/lzw smieci/tad_${algo}.enc smieci/tad_${algo}.dec d -a ${algo}
        echo
done
