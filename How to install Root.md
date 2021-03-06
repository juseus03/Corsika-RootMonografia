#How to install ROOT in UBUNTU 16.04LTS

This file explains how to install ROOT (data analys framework from CERN) in Ubuntu 16.04LTS and how to sort some installation problems.
A good installation guide is at: https://root.cern.ch/building-root and the README file in the source.

1. Install CMake:
    ```
    sudo apt-get install cmake
    ```
2. Get the source file at: 
    * https://root.cern.ch/downloading-root
    * the file is named as: root_`<version>`_source.tar.gz
3. Extract the source file.

4. Make a new directory for installation, ROOT source directory doesn't work.
    ```
    mkdir <builddir>
    cd <builddir>
    ```
5. Execute the following commands:
    ```
    cmake ../root
    cmake --build .
    ```  
  For version 6.06.08 of root it appears the error:
   * *Found GCC compiler with new ABI.  Cling does not support the GCC 5 ABI yet"*
   
  This can be solved by executing the following commands, this solutions is based on 
  **ferhue** solution at https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=21043&p=91452#p91452 and http://askubuntu.com/questions/26498/choose-gcc-and-g-version/26518#26518
  ```
  sudo apt-get install gcc-4.8 g++-4.8
  sudo update-alternatives --remove-all g++
  sudo update-alternatives --remove-all gcc
  sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 10
  sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 20
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 10
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 20
  sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
  sudo update-alternatives --set cc /usr/bin/gcc
  sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
  sudo update-alternatives --set c++ /usr/bin/g++
  sudo update-alternatives --config gcc (choose option 4.8)
  sudo update-alternatives --config g++ (choose option 4.8)
  ```
  If you execute this commands, both answers must be 4.8
    ```
    gcc --version
    g++ --version
    ```
6. Now you can re execute:
    ```
    cmake ../root
    cmake --build .
    ```
7. Once cmake has finished (it takes a while) Add bin/ to PATH and lib/ to LD_LIBRARY_PATH. 
    * For the sh shell :
        ```
        . bin/thisroot.sh
        ```
    * For the csh shell do:
        ```
        source bin/thisroot.csh
        ```
8. Now you should be able to run ROOT:
      ```
        root
      ```
9. A lot of tutorials on how to use ROOT are in:

    https://root.cern.ch/courses
    
