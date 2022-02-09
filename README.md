# sis
SIS is an interactive tool for synthesis and optimization of sequential circuits. However, during my use of **sis**, it can be invoked in my code, i.e., there is a problem with **sis -c sis_command**. I modify the initial code of **sis/main/sis.c** such that it can be invoked, e.g., **sis -c "read_blif test.blif;print_stats;"**.
## download
```
git clone https://github.com/FCHXWH/sis.git
cd sis
```

## usage
Install necessary dependencies:  
```
sudo apt install -y make gcc bison flex build-essential
```
and then run:
```
./configure prefix=/usr/local
make
sudo make install
```
