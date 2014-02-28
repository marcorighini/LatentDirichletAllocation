#!/bin/bash

training=([0]='../data/enron/enron_data/enron_0.dat' [1]='../data/enron/enron_data/enron_1.dat' [2]='../data/enron/enron_data/enron_2.dat' [3]='../data/enron/enron_data/enron_3.dat')
validation=([0]='../data/enron/enron_data/enron_0_val.dat' [1]='../data/enron/enron_data/enron_1_val.dat' [2]='../data/enron/enron_data/enron_2_val.dat' [3]='../data/enron/enron_data/enron_3_val.dat')

topics=(50)

for (( i_topic=0; i_topic<${#topics[@]}; i_topic++ ))
do
	topic="${topics[i_topic]}"
	output='../data/enron/enron_'$topic
	mkdir $output
done	

for (( i_train=0; i_train<${#training[@]}; i_train++ ))
do
	train_name="${training[i_train]}"
	validation_name="${validation[i_train]}"

	for (( i_topic=0; i_topic<${#topics[@]}; i_topic++ ))
	do
		topic="${topics[i_topic]}"
		output='../data/enron/enron_'$topic
		
		
		lda='../Debug/LDA --train '$train_name' --validation '$validation_name' --output '$output' --iters '1000' --topics '$topic' --perpl' 
		echo ' '		
		echo '*******  COMANDO: '$lda
		$lda
	done	
done
