
alias make="make $1 -j10"

aoc1(){
        echo ${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/test1.txt
}

aoc2() {
        echo ${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/input.txt
}

aocx(){
        echo ${1}/${2}/build/bin/${1}day${2}
}

valgarg(){
        valgrind --leak-check=full -s --fullpath-after=. $1 $2 $3
}

aocvalt() {
        valgarg $(aoc1 $1 $2) $3 $4
}

aocvali(){
        valgarg $(aoc2 $1 $2) $3 $4
}

aocvalht(){
        valgrind --tool=dhat $(aoc1 $1 $2) $3 $4
}

aocvalhi(){
        valgrind --tool=dhat $(aoc2 $1 $2) $3 $4
}

aoctest() {
        ${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/test1.txt ${3} ${4}
}

aocinput(){
        ${1}/${2}/build/bin/${1}day${2} ${1}/${2}/data/input.txt ${3} ${4}
}

aocdbgtest() {
        gdb --args $(aoc1 $1 $2) $3 $4
}

aocdbginput() {
        gdb --args $(aoc2 $1 $2) $3 $4
}

aoctestall(){
        for year in $(seq 2022 2025);
        do
                for day in $(seq -f "%03g" 000 025);
                do
                        if [ -f $(aocx $year $day) ]; then
                                aoctest $year $day --no-draw
                                aocinput $year $day --no-draw
                        fi
                done
        done
}

aocclean(){
        make clean
        find -name "CMakeCache.txt" -type f -delete
        find -name "Makefile" -type f -delete
        find -name "Testing" -type d -execdir 'rm' '-rv' '{}' ';'
        find -name "DartConfiguration.tcl" -type f -delete
        find -name "CTestTestfile.cmake" -type f -delete
        find -name "cmake_install.cmake" -type f -delete
        find -name "build" -type d -execdir 'rm' '-rv' '{}' ';'
        find -name "CMakeFiles" -type d -execdir 'rm' '-rv' '{}' ';'
}

_aoc_opts-2 ()
{
 local cur
 COMPREPLY=()
 cur=${COMP_WORDS[COMP_CWORD]}

 case "$cur" in
        -*)
        COMPREPLY=($(compgen -W '--no-draw --draw-delay --' -- $cur));;
 esac
 return 0
}

complete -F _aoc_opts-2 -o filenames aoctest 
complete -F _aoc_opts-2 -o filenames aocinput 
