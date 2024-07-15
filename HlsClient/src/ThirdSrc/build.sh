#!/bin/bash

function PrintUsage()
{
    echo -e "Usage:"
    echo -e "./build.sh $ToolChain"
    echo -e "ToolChain: arm-linux/x86/x64"
    echo -e "EGG:"
    echo -e "./build.sh x86"
    echo -e " or ./build.sh x64"
}

if [ $# == 0 ]; then
    PrintUsage
    exit -1
else
    cd $2
    if [ -e "ThirdLib" ]; then
        echo "ThirdLib exit"
    else
        mkdir ThirdLib
    fi
    cd -
    cd media
    sh build.sh $1 $2/ThirdLib
    if [ $? -ne 0 ]; then
        exit -1
    fi
    cd ..

    cd http
    sh build.sh $1 $2/ThirdLib
    if [ $? -ne 0 ]; then
        exit -1
    fi
    cd ..

fi




