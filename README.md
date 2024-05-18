# dsi-storage

Tools to encrypt/decrypt dsi partition with TO136

Requirement : 

    sudo apt-get install cryptsetup

Compilation : 

    make

Usage to encrypt :

    ./encrypt-dsi-storage <partition_path>
    
ex : ./encrypt-dsi-storage /dev/mmcblk2p2

Usage to decrypt :

    ./decrypt-dsi-storage <partition_path> <mapper_name>
    
ex : ./decrypt-dsi-storage /dev/mmcblk2p2 dsi

# To compile and install libto :

    $ cd libto
    $ rm -rf build; mkdir build
    $ autoreconf -f -i
    $ cd build
    $ ../configure i2c=linux_generic i2c_dev=/dev/i2c-2 --prefix=/
    $ make
    $ make install
