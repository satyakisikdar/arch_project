# arch_project readme

```main.cpp``` command line flags
1. ```--neurons``` - number of neurons in the hidden layer, default: 100
2. ```--alpha1``` - learning rate for the first layer, default: 0.2
3. ```--alpha2``` - learning rate for the second layer, default: 0.1
4. ```--maxcond``` - maximum conductance, default: 5e-6 
5. ```--mincond``` - minimum conductance, default: 100e-9
6. ```--condlevels``` - number of conductance levels, default: 63
7. ```--nonlinear``` - nonlinearity, default: 2.4
8. ```--online``` - turns on online learning, disabled by default
9. ```--ideal``` - switches to ideal device, disabled by default 

Please double check the default values for the IdealDevice. Remember to properly initialize ```arrayIH->Initialization<>``` and ```arrayHO->Initialization<>``` in main. 
