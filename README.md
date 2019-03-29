# SAP1 Virtual Machine

Virtual machine for the Simple As Possible 1, a computer devised by Albert P.
Malvino in his book "Digital Computer Electronics" and further implemented by
Ben Eater in his YouTube series, "Building an 8-bit Breadboard Computer".

The VM is currently in a working state and can be compiled by simply changing
into the directory and executing `make`.

```
cd sap1
make
```

TODO:
- Save the VM state to file.
- Load the VM state from file.
- Save a VM program to file.
- Load a VM program from file.
- Write a simple assembler for VM programs.
- Rewrite the `ex_add` and `ex_sub` functions to mimic the way the physical
machine handles the `ADD` and `SUB` instructions.
- Add a graphical user interface.
