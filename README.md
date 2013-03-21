libNBComms
==========

C-library to facilitate communication with nodejs Ninja Blocks modules

Compiling
---------
    cd libNBComms
    make
Installing
----------
    sudo make install

* This copies headers (./include/nbsocket.h) to <client>/drivers/common/include/
* Moves libnbcomms.so to <client>/drivers/common/lib/linux/   (making conventional links too)
* adds the library path to the /etc/lib.so.conf (blindly adds the line each install)
* runs ldconfig to update linker library paths
