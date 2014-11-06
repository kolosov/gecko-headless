gecko-headless
==============
This is an applicatation based on gecko-embedding project to control gecko embedded browser by socket. The final goal is a headless gecko-based web-scrapping tool.

Complilation
------------
    clone repo(git clone https://github.com/kolosov/gecko-headless.git) && cd gecko-headless
    git submodule init
    git submodule update
    mkdir ../build && cd ../build
    cmake ../gecko-headless -DGECKO_SDK_PATH=/home/usr/xulrunner-sdk-31
    make
