#!/bin/bash
TEST=$1 #ScreenTest
COMMAND=$2 #ScreenTest
HOST=$3 #192.168.1.135
PORT=$4 #10023
USER=$5 #amigaX1000
FTPPASSWORD=$6 #amigaftp
TELNETPASSWORD=$7 #amigatelnet
COMPARE=$8 #Opening successful

function ftp_file() {
ftp -inv $HOST <<EOF
user $USER $FTPPASSWORD
binary
rm $1
put $1
chmod +x $1
exit
EOF
}
#note: chmod does not work on Amiga

function assert() {
    if [[ $1 =~ $2 ]]; then
        return 0
    else
        return 1
    fi
}

function run_test() {
    expect runonremote.expect $HOST $PORT $TELNETPASSWORD "$1"
}

function test() {
    ftp_file $1 >/dev/null 2>&1
    run_test $1 >/dev/null 2>&1
    result=""
    if [ -f result.txt ]; then
        result=$(cat result.txt)
    fi

    #if cat result.txt | grep -q "$2"
    if assert "$result" "$2"
    then
        echo "PASSED"
    else
        echo "FAILED"
    fi
}

echo -n "TEST $1... "; test $COMMAND $COMPARE

#echo -n "Running ScreenTest... "; test ScreenTest "Opening successfull.*Screen name: EasyDebug.*Dimensions:.*"
#echo -n "Running ReqTest... "; test ReqTest "Ready."
#echo -n "Running StringTest... "; test StringTest "ready?"
#echo -n "Running ProgressTest... "; test ProgressTest "Ready."
#echo -n "Running ButtonBarTest... "; test ButtonBarTest "returned an object"
#echo -n "Running ListbrowserTest... "; test ListbrowserTest "All OK"
#echo -n "Running ButtonTest... "; test ButtonTest "OK"
#echo -n "Running LayoutTest... "; test LayoutTest "OK"
#echo -n "Running WidgetTest... "; test WidgetTest "OK"
#echo -n "Running MenuTest... "; test MenuTest "OK"
#echo -n "Running ActionBarTest... "; test ActionBarTest "OK"
#echo -n "Running GuiTest... "; test GuiTest "OK"
