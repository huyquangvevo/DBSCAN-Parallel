
mpic++ -o dbscan-mpi grid_dbscan_mpi.cpp


for i in {1..10}
do 
    for datafile in 1 5 10 15 20
    do
        for np in 2 3 4 
        do 
            mpirun --hostfile /etc/hosts -np ${np} dbscan-mpi Circle_${datafile} > output/Iter${i}_np${np}_datafile${datafile}.txt
        done 
    done
done 


g++ -o dbscan-ori1 grid_dbscan_original.cpp 

for i in {1..3}
do
    for datafile in 1 5 10 15 20
        do
        for np in 2 3 4 
        do 
            ./dbscan-ori1 Circle_${datafile} > output_ori/Iter${i}_ori_datafile${datafile}.txt
        done 
    done
done 


g++ -o dbscan-ori2 grid_dbscan_original.cpp 


for i in {4..6}
do
    for datafile in 1 5 10 15 20
        do
        for np in 2 3 4 
        do 
            ./dbscan-ori2 Circle_${datafile} > output_ori/Iter${i}_ori_datafile${datafile}.txt
        done 
    done
done 


