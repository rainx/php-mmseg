#!/bin/bash

PWD=`pwd`

cd mmseg
phpize
./configure --with-mmseg=/opt
make
sudo make install

cd $PWD
