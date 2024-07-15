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
    CopyPwd=$PWD/build/linux/$1
    cd src
    sh build.sh $1 $CopyPwd
    if [ $? -ne 0 ]; then
        exit -1
    fi
    cd ..

    cd demo
    sh build.sh $1 $CopyPwd
    if [ $? -ne 0 ]; then
        exit -1
    fi
    cd ..

fi




