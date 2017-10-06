# BasicBlocks
Pin tool for printing the address of each basic block executed in a program.

## Compiling
Change directories to your pin installation folder, then navigate to source/tools.
```
git clone https://github.com/chokepoint/BasicBlocks.git
cd BasicBlocks
make
pin -t obj-intel64/BasicBlocks.so -- /bin/ls > /tmp/ls.ptrace
```
