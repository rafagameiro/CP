#!/bin/bash

#Type of pattern to run the program;
PATTERN="ALL"; 

#Number of threads to test the program
THREADS=1;

#ID of the test to run
#Can be used later to separate more tests
TEST_ID=; 

#Number of interations to compute the mean value
NUM_ITERATION=5

#Number of generations to each test
TEST_GEN=(100000 1000000 10000000 100000000)

#Name of the tests the program is testing
TEST_NAMES=()

#Array that contains all the possible patterns the program can execute
declare -A PATTERNS=([MAP]=2 [STENCIL]=3 [REDUCE]=2 [SCAN]=2 [PACK]=3 [GATHER]=3 [SCATTER]=12 [PIPELINE]=3 [FARM]=3 [ALL]=9)

declare -A TEST_RESULTS
declare -A RESULTS_SEQ

#Fill the array "TEST_NAMES" with the names of the tests the program is running
fill_array() {
    
    #Goes through all the input received as argument
    for(( i = 0; i < $2; i++))
    do
        #For each line parses the input to obtain only the name of the test for each line
        if [ $i -eq 0 ]
        then
            TEST_NAMES[$i]=$(echo $1 | cut -d":" -f$( expr $i + 1))
        else
            TEST_NAMES[$i]=$(echo $1 | cut -d":" -f$( expr $i + 1) | cut -d"|" -f4)
        fi        
    done
}

#Replaces all values in the array "TEST_RESULTS" with zeroes
clear_array() {

    local i;
    local j;
    for(( i = 0; i < ${#TEST_NAMES[@]}; i++))
    do
        for(( j = 0; j < ${#TEST_GEN[@]}; j++))
        do
            TEST_RESULTS[$i, $j]=$(echo "scale=2; 0.00" | bc)
        done
    done

}

#Copies the values computed during the sequential execution of the program, 
#saved in the array "TEST_RESULTS", to the auxiliar array "RESULTS_SEQ"
copy_results_seq() {

    for((i = 0; i < ${#TEST_NAMES[@]}; i++))
    do
        for((j = 0; j < ${#TEST_GEN[@]}; j++))
        do
            RESULTS_SEQ[$i, $j]=${TEST_RESULTS[$i, $j]}
        done
    done
}

#Parses the received results from an execution and places inside the array "TEST_RESULTS"
process_tests() {
    
    #If the "TEST_NAMES" is empty, which only happens when the function is executed for the first time
    #It will fill the array the names of the tests
    if [ ${#TEST_NAMES[@]} -eq 0 ]
    then
        str=$(echo $2 | tr " " "|")
        fill_array $str ${PATTERNS[$PATTERN]}
        clear_array
    fi
    
    #For each test name, saved in the "TEST_NAMES"
    for(( i = 0; i < ${#TEST_NAMES[@]}; i++))
    do
        #crops the output given by the tests and saves it in a position inside the array "TEST_RESULTS"
        #the position corresponds to the (name_of_test, number_of_generation)
        out=$(echo $2 | cut -d ':' -f $(expr $i + 2) | cut -d 's' -f1 )
        TEST_RESULTS[$i, $1]=$(echo "scale=2; ${TEST_RESULTS[$i, $1]} + $out" | bc)
    done
    
}

#Sets the value of the variable "PATTERN" to later be used to run the tests
choose_pattern() {

    #If the typed pattern is in the array "PATTERNS"
    if [ -v PATTERNS[$1] ]
    then #the "PATTERN" is set with the name of the pattern typed
        PATTERN=$1
    else #if not the variable remains empty
        PATTERN="ALL";
    fi

}

#Processes the arguments typed by the user
process_args() {

    FUNC=process_tests

    #Goes through all the arguments
    for (( i=1; i<=$#+1; i++)); do

        #If there is an argument with an initial flag "-p"
        if [ "${!i}" == "-p" ]
        then #It will verify is the next argument, which should be a pattern name, exists
            j=$((i+1))
            choose_pattern ${!j^^}
        
        #If there is an argument with an inital flag "-t"
        elif [ "${!i}" == "-t" ]
        then #Processes the next argument(if correctly typed), and stores the number threads to run the tests
            j=$((i+1))
            THREADS=$(echo ${!j} | tr ',' '\n')
        fi
    done

}

#Computes the mean of values for each test
compute_mean() {

    #Goes through each test
    for((i = 0; i < ${#TEST_NAMES[@]}; i++))
    do
        #For each test generation value
        for((j = 0; j < ${#TEST_GEN[@]}; j++))
        do
            #Computes the mean of values, dividing the sum of values stored in the array "TEST_RESULTS" by the number of iteration done
            TEST_RESULTS[$i, $j]=$(echo "scale=2; ${TEST_RESULTS[$i, $j]} / $NUM_ITERATION" | bc)
        done
    done

}

#Computes the speedup between the current set of tests done with a specific number of threads 
#and the sequential version intially computed
compute_speedup() {

    local OUTPUT=$(printf "Speedup between the sequential version and the parallel version, %d threads." $1)

    #Goea through all tests
    for((i = 0; i < ${#TEST_NAMES[@]}; i++))
    do
        #For each test generation value
        for((j = 0; j < ${#TEST_GEN[@]}; j++))
        do
            #If the value obtained in the parallel version tests are smaller or equal to zero
            if [ $(echo "${TEST_RESULTS[$i, $j]}>0" |bc) -eq 0 ]
            then #The speedup is the value of the "RESULTS_SEQ" in the position (i, j)
                TEST_RESULTS[$i, $j]=${RESULTS_SEQ[$i, $j]}
            else #The speedup is the division between the sequential value in the position (i, j)
                 #and the parallel version calculated, in the same position 
                TEST_RESULTS[$i, $j]=$(echo "scale=2; ${RESULTS_SEQ[$i, $j]} / ${TEST_RESULTS[$i, $j]}" | bc)
            fi
        done
    done

    print_results $OUTPUT
}

#Runs all the Program and call the function to store the outputed values
run_tests() {

    #For each test generation value
    for((j = 0; j < ${#TEST_GEN[@]}; j++))
    do
        #It will execute the same tests a "NUM_ITERATION" of times
        #in order ot compute the mean
        for(( k = 0; k < $NUM_ITERATION; k++))
        do
            #If the value of "PATTERN" is "ALL"
            if [ "$PATTERN" == "ALL" ]
            then #executes the program will a default set of tests
                out="$(OMP_NUM_THREADS=$2 ./$1 ${TEST_GEN[$j]})"
            else #executes the program will a set of tests,
                 #just for the typed pattern
                out="$(OMP_NUM_THREADS=$2 ./$1 -p $PATTERN ${TEST_GEN[$j]})"
            fi
            #Stores the outputed values in an array
            $FUNC $j "$(echo $out | cut -d '!' -f 2)"
        done
    done    
    
    compute_mean
}

#Prints the results obtained after performing a set of tests
print_results() {

    #Prints a title decribing the test setup
    echo $@
    printf "\t\t\t\t"

    #Prints the tests names 
    for(( i = 0; i < ${#TEST_GEN[@]};i++))
    do  
        printf "%d\t" ${TEST_GEN[$i]}
        if [ $(expr $i + 2) -ne ${#TEST_GEN[@]} ]
        then
            printf "\t"
        fi
    done
    printf "\n"


    #For each test
    for((i = 0; i < ${#TEST_NAMES[@]}; i++))
    do
        #Prints the specific test
        printf "%s\t\t" ${TEST_NAMES[$i]}
        if [ ${#TEST_NAMES[$i]} -le 15 ]
        then
            printf "\t"
        fi

        #Goes through all test generation values
        for((j = 0; j < ${#TEST_GEN[@]}; j++))
        do
            #prints the value stored in the position (i, j)
            printf "%.4f\t\t" ${TEST_RESULTS[$i, $j]}
        done
        printf "\n"
    done
    printf "\n"

}

#Executes the sequential version of tests
run_seq() {

    local FILE="main-seq"
    local OUTSTREAM="Results of tests using the Sequential version."

    #Runs the tests and store its values in the array "TEST_RESULTS"
    run_tests $FILE

    #Prints the obtained values
    print_results $OUTSTREAM

    #Copies the values to the auxiliar array "RESTULS_SEQ"
    copy_results_seq

    #And resets the array "TEST_RESULTS"
    clear_array

}

#Executes the paralle version of tests
run_par() {

    local FILE="main-par"
    local OUTSTREAM="Results of tests using %d threads."

    #For each number of threads typed by the user
    for t in $THREADS
    do
        #Runs the tests and store its values in the array "TEST_RESULTS"
        run_tests $FILE $t

        #Prints the obtained results
        print_results $(printf "Results of tests using %d threads." $t)

        #Computes the speedup between this set of tests and the sequential version
        # previously computed
        compute_speedup $t

        #And resets the array "TEST_RESULTS"
        clear_array
    done

}

print_help() {
    
    printf "Usage: ./script.sh [-p pattern] [-t threads-num]\n\n"
    printf "[-p pattern]:\t\t this flag will allow the script to only run the tests associated with the typed test.\n"
    printf "[-t thread-num]:\t this flag will execute the requested tests with the amount of threads specified.\n\n"
    printf "Example: ./script.sh -p map -t 3,4\n"
    exit 2

}

#If the number of typed arguments is greater than 5
if [ $# -ge 5 ]
then #Prints and example on how to execute the script
    echo "Error: wrong number of arguments inserted!"
    echo "Type ./script.sh -h to know how to execute."
    exit 2
fi

#If the first argument is -h
if [ "$1" == "-h" ]
then #Prints and example on how to execute the script
    print_help
fi

#Depending on the number of arguments typed
#The process of the arguments will be bigger
case $# in
    0)
        #TEST_ID=$1      
        FUNC=process_tests
    ;;
    
    1)
        process_args $1
        #TEST_ID=$2
    ;;
    
    2)
        process_args $1 $2
        #TEST_ID=$3
    ;;
    
    3)
        process_args $1 $2 $3
        #TEST_ID=$4
    ;;
    
    4)
        process_args $1 $2 $3 $4
        #TEST_ID=$5
    ;;

esac 

#Starts executing the tests
run_seq
run_par

