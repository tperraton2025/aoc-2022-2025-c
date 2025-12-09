
aoc1(){
	echo ${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/test1.txt
}

aoc2() {
	echo ${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/input.txt
}

aocx(){
	echo ${1}/${2}/build/bin/${1}day${2}
}

aocvalt() {
	valgrind --leak-check=full -s --track-origins=yes $(aoc1 $1 $2)
}

aocvali(){
	valgrind --leak-check=full -s --track-origins=yes $(aoc2 $1 $2)
}

aoctest() {
	${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/test1.txt
}

aocinput(){
	${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/input.txt
}

aocdbgtest() {
	gdb --args $(aoc1 $1 $2)
}

aocdbginput() {
	gdb --args $(aoc2 $1 $2) 
}

aoctestall(){
	for year in $(seq 2022 2025);
	do
		for day in $(seq -f "%03g" 000 025);	
		do
			if [ -f $(aocx $year $day) ]; then
				aoctest $year $day
			fi
		done
	done
}













