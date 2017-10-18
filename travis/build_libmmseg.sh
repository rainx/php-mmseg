#!/bin/bash

PWD=`pwd`

cd dependencies/mmseg-3.2.14
./bootstrap
./configure --prefix=/opt/
make 
sudo make install

cd $PWD
