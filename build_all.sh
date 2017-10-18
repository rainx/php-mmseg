#!/bin/bash

travis/build_libmmseg.sh
travis/build_php-mmseg.sh

echo Please add mmseg/ini/mmseg.ini to Your php config path
