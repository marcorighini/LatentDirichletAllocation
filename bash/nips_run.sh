#!/bin/bash

training=([0]='../data/nips/nips_data/nips_0.dat' [1]='../data/nips/nips_data/nips_1.dat' [2]='../data/nips/nips_data/nips_2.dat' [3]='../data/nips/nips_data/nips_3.dat')
validation=([0]='../data/nips/nips_data/nips_0_val.dat' [1]='../data/nips/nips_data/nips_1_val.dat' [2]='../data/nips/nips_data/nips_2_val.dat' [3]='../data/nips/nips_data/nips_3_val.dat')

topics=(25 50 100 150 200)

for (( i_topic=0; i_topic<${#topics[@]}; i_topic++ ))
do
	topic="${topics[i_topic]}"
	output='../data/nips/nips_'$topic
	mkdir $output
		
	for (( i_train=0; i_train<${#training[@]}; i_train++ ))
	do
		train_name="${training[i_train]}"
		validation_name="${validation[i_train]}"

		lda='../Debug/LDA --train '$train_name' --validation '$validation_name' --output '$output' --iters '800' --topics '$topic' --perpl'
		echo ' '		
		echo '*******  COMANDO: '$lda
		$lda
	done	
done
