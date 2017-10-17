#!/bin/bash

PWD=`pwd`

cd php-mmseg/mmseg
phpize
./configure --with-mmseg=/opt
make
sudo make install

cd $PWD
