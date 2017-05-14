#!/bin/bash
export SAVEDIR=$PWD; 
cd $HOME/corsika-75600/run/;

#Display usage method for information
display_usage() { 
	echo "start_simulation.sh [time (Min)]"
    echo "time > 0 "
	} 

#Checks if user ask for help
if [[ ( $1 == "--help") ||  $1 == "-h" ]] 
	then 
		display_usage
		exit 0
	fi

re='^[0-9]+$'
if ! [[ $1 =~ $re ]] ; then
   display_usage
   exit 1
fi    
    
#Condition if time is less than 0
if [  $1 -le 0 ] 
	then 
		display_usage
		exit 1
	fi     

#Start simulation if arguments are good
MAXLIM=$1;
for j in $(eval echo {1..$MAXLIM}); do
    for i in $(ls $SAVEDIR/inputs_Z*_$j); do
        ./corsika75600Linux_QGSJET_gheisha_root <$i>""$i"output.txt" & 
    done
    wait
    rm $SAVEDIR/*.long;
done

