#!/bin/bash

sudo apt-get install -y gcc g++ python gcc g++ python python-dev mercurial python-setuptools git qt5-default apt-get install python-pygraphviz python-kiwi python-pygoocanvas libgoocanvas-dev ipython openmpi-bin openmpi-common openmpi-doc libopenmpi-dev autoconf cvs bzr unrar gdb valgrind uncrustify doxygen graphviz imagemagick texlive texlive-extra-utils texlive-latex-extra texlive-font-utils texlive-lang-portuguese dvipng latexmk python-sphinx dia gsl-bin libgslcblas0 libgsl-dev flex bison libfl-dev tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev cmake libc6-dev libc6-dev-i386 libclang-dev llvm-dev automake
sudo pip install cxxfilt
sudo apt-get install libgtk2.0-0 libgtk2.0-dev vtun lxc libboost-signals-dev libboost-filesystem-dev
wget https://www.nsnam.org/releases/ns-allinone-3.29.tar.bz2
tar -xvf ns-allinone-3.29.tar.bz2
cd './ns-allinone-3.29/'
sudo ./build.py
