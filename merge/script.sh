
mpic++ -o merge-mpi merge_mpi.cpp

mkdir output

mkdir output_ori


for i in {1..10}
do 
    for datafile in 100 1000 5000 10000 100000 200000 1000000 10000000
    do
        for np in 2 3 4 
        do 
            mpirun --hostfile /etc/hosts -np ${np} merge-mpi ${datafile} > output/Iter${i}_np${np}_N${datafile}.txt
        done 
    done
done 




for i in {1..10}
do 
    for datafile in 100 1000 5000 10000 100000 200000 1000000 10000000
    do
            ./merge-ori ${datafile} > output_ori/Iter${i}_ori_N${datafile}.txt
    done
done 
