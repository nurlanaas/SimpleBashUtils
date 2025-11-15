#!/bin/bash 
#это путь к Bash-интерпретатору

SUCCESS=0
FAIL=0
COUNTER=0
DIFF_RES=""

#встроенная команда,которая позволяет объявлять и устанавливать переменные с определёнными атрибутами
#например:Указывать, что переменная — массив
declare -a tests=( 
"VAR s test_0_grep.txt"
#Поиск строки "for" в трёх файлах
"VAR for ../grep.c ../grep.h ../Makefile"
"VAR for ../grep.c"
#Ищет либо "for", либо строки, начинающиеся с "int" во всех трёх файлах
"VAR -e for -e ^int ../grep.c ../grep.h ../Makefile"
"VAR -e for -e ^int ../grep.c"
"VAR -e regex -e ^print -f test_ptrn_grep.txt ../grep.c"
"VAR -e while -e void -f test_ptrn_grep.txt ../grep.c ../Makefile "
)

#объявление функции
testing()
{
    #$@ — это все аргументы, переданные в функцию (например: "VAR s test_0_grep.txt")
    #Берёт аргументы ($@),Заменяет VAR на значение переменной $var (-v, -c, -l) с помощью команды sed
    #sed - потоковый текстовый редактор, позволяет искать, заменять и модифицировать текст 
    #sed "s/что_искать/на_что_заменить/"
    t=$(echo $@ | sed "s/VAR/$var/")
    ../s21_grep $t > test_s21_grep.log
    grep $t > test_sys_grep.log
    DIFF_RES="$(diff test_s21_grep.log test_sys_grep.log)"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "" ]
    then
      (( SUCCESS++ ))
      #Использует ANSI-коды \033[31m red \033[32m - green \033[0m - сброс к норм цвету
        echo -e "\033[32msuccess\033[0m \ngrep $t"
    else
      (( FAIL++ ))
        echo -e " \033[31mfail\033[0m\n grep $t"
    fi
    rm test_s21_grep.log test_sys_grep.log
}


# 1 параметр
for var1 in v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# # 2 параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

# # 3 параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        for var3 in v c l n h o
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    testing $i
                done
            fi
        done
    done
done

# # 2 сдвоенных параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                testing $i
            done
        fi
    done
done

# # 3 строенных параметра
for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        for var3 in v c l n h o
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1$var2$var3"
                    testing $i
                done
            fi
        done
    done
done

echo -e "\033[31mFAIL: $FAIL\033[0m"
echo -e "\033[32mSUCCESS: $SUCCESS\033[0m"
echo -e "ALL: $COUNTER"
