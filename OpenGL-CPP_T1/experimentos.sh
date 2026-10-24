#!/bin/bash
rm -f benchmark.csv
for N in 1000 10000 50000; do
  for TAM in 0.1 0.5 2.0; do
    for NIVEL in 3 5 7; do
      for SEED in 1 2; do
        echo "N=$N tam=$TAM nivel=$NIVEL semente=$SEED"
        xvfb-run -a ./BasicoOpenGL $N $SEED $TAM $NIVEL bench > /dev/null
      done
    done
  done
done
echo "Pronto: $(wc -l < benchmark.csv) linhas em benchmark.csv"
