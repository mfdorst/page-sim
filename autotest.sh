#!/bin/bash

TESTDIR=test_cases

if [ ! -d $TESTDIR ]
then
	echo "Your TESTDIR appears to be misconfigured. Edit the variable TESTDIR at the top of the file $BASH_SOURCE to be the location of the test cases you got from canvas"
	echo "TESTDIR is set to '$TESTDIR'. No such directory exists."
	echo ""
fi

if [ $BASH_SOURCE = $0 ]
then
	echo "YOU ARE TRYING TO EXECUTE THIS SCRIPT.  You should source it instead."
	echo "Try running 'source $BASH_SOURCE' or '. $BASH_SOURCE' (make sure to include the space between the period and $BASH_SOURCE)"
	echo "Your test directory is: $TESTDIR"
	echo "After sourcing this file and configuring the TESTDIR you can use the commands below"
	echo ""
	echo "TO TEST RUN THE FOLLOWING TESTING COMMANDS IN THE SAME DIRECTORY THAT YOU HAVE WRITTEN YOUR CODE IN:"
	echo "Command descriptions:"
	echo "jtest: compiles your JAVA implementation of the assignment and runs all the tests on it"
	echo "ctest: compiles your C implementation of the assignment and runs all the tests on it"
	echo "cpptest: compiles your C++ implementation of the assignment and runs all the tests on it"
	echo "runalltests (java|c|cpp): DOES NOT COMPILE YOUR CODE. You must pass in the language you wrote your code in. Runs all tests on your code"
	echo "run1test (java|c|cpp) <testpath>: DOES NOT COMPILE YOUR CODE. You must pass in the language you wrote your code in. Runs the test at testpath."
	exit 1
fi

jtest ()
{
	javac *.java
	if [ $? != 0 ]
	then
		echo "error did not compile"
		return
	fi

	runalltests java
}

ctest ()
{
	gcc -std=c99 *.c -o program_under_test
	if [ $? != 0 ]
	then
		echo "error did not compile"
		return
	fi

	runalltests c
}

cpptest ()
{
	g++ *.cpp -o program_under_test
	if [ $? != 0 ]
	then
		echo "error did not compile"
		return
	fi

	runalltests cpp
}

runalltests ()
{
	if [ ! -d $TESTDIR ]
	then
		echo "Your TESTDIR appears to be misconfigured. Edit the variable TESTDIR at the top of the autotest file you sourced to be the location of the test cases you got from canvas"
		echo "TESTDIR is set to '$TESTDIR'. No such directory exists."
		echo ""
		return
	fi

	for f in $TESTDIR/*
	do
		if [ -f "${f%.*}o.${f##*.}" ]
		then
			run1test $1 $f
		fi
	done
}

run1test ()
{
	#get the command
	local runcom
	if [ $1 = "java" ]
	then
		local mf=`grep -rEl "public\s*static\s*void\s*main"`
		if [ `grep -rE "public\s*static\s*void\s*main" | wc -l` != 1 ]
		then
			echo "could not find main class/found too many main classes"
			return 1
		fi
		runcom="java ${mf%.java}"
	elif [ $1 = "c" ] || [ $1 = "cpp" ]
	then
		runcom="./program_under_test"
	else
		echo "Did not recognize language"
	fi

	#check if there are proper input and output files
	local of="${2%.*}o.${2##*.}"
	if [ -f $2 ] || [ -f $of ]
	then
		echo "Testing the program with input file '$2' and test output '$of' and run command '$runcom'"
		if [ -f input.txt ]
		then
			rm input.txt
		fi
		ln -s -T $2 input.txt
		$runcom
		diff -i output.txt $of
		if [ $? = 0 ]
		then
			echo "Test passed"
		fi
	else
		echo "No input and test output pair '$2' and '$of'"
	fi
}
