
![license](https://img.shields.io/github/license/ric2k1/qsyn?style=plastic)
![stars](https://img.shields.io/github/stars/ric2k1/qsyn?style=plastic)
![contributors](https://img.shields.io/github/contributors/ric2k1/qsyn?style=plastic)
![pr-welcome](https://img.shields.io/badge/PRs-welcome-green?style=plastic)
![g++-10](https://img.shields.io/badge/g++-≥10-blue?style=plastic)
![gfortran-10](https://img.shields.io/badge/gfortran-≥10-blueviolet?style=plastic)

# Qsyn: An End-to-End Quantum Circuit Synthesis Framework
![](https://i.imgur.com/wKg5cQO.jpg)


![](https://i.imgur.com/jxLKFko.png)

<!-- ![example branch parameter](https://github.com/ric2k1/qsyn/actions/workflows/build-and-test.yml/badge.svg)
 -->

## Introduction
Qsyn is a C++ based growing software system for synthesis, optimization and verification of quantum circuits appearing in quantum computers. Qsyn implements scalable quantum circuits optimization by combining ZX-Calculus and technology mapping.

Qsyn provides an experimental implementation of optimization algorithms and a programming environment for simulation or building similar applications. Future development will focus on enhancing the algorithms, and visualization of ZX-graphs.

## Third party dependencies
* xtensor, xtensor-blas
* pdflatex

## Getting Start
### Installation
```shell!
git clone https://github.com/ric2k1/qsyn.git
cd qsyn
```
#### Visualization (optional for ZXGDraw)
```shell
sudo apt-get install texlive-latex-base
```
### Compilation
`Qsyn` requires at least `g++-10` and `gfortran-10` to compile. The two compilers should also be on the same version.

1. Qsyn depends on `xtensor` and `xtensor-blas`. To install these dependencies, run `configure.sh`, which checks for lacking dependencies and install them automatically. 

	```shell!
	./configure.sh
	```
2. Then, run `make` to build up the executable.
	```shell!
	make -j16
	```
3. If the compilation process ends successfully, you will get
    ```shell!
    > building qsyn...
    ~/qsyn$ _
    ```
4. If you want to delete all intermediate files created in the compilation process, please type
    ```shell!
	make clean
	```

### Run

* After successful compilation, you can call the command-line interface of `Qsyn` where you can execute commands implemented into `Qsyn`.
    
   ```shell!
    ❯ ./qsyn
    DV Lab, NTUEE, Qsyn 0.4.0
    qsyn> 
   ```


* To run the demo program, you can provide a file containing commands. For example:
    ```shell!
    ❯ ./qsyn -f tests/demo/demo/dof/tof_3.dof
    DV Lab, NTUEE, Qsyn 0.4.0
    qsyn> verb 0
    Note: verbose level is set to 0

    qsyn> zxgread benchmark/zx/tof3.zx

    qsyn> zxgs -freduce

    qsyn> zxgp
    Graph 0( 3 inputs, 3 outputs, 17 vertices, 19 edges )

    qsyn> qq -f
    ```

* The same result can be produced by running in the command-line mode:
    ```shell!
    ❯ ./qsyn
    DV Lab, NTUEE, Qsyn 0.4.0
    qsyn> dofile tests/demo/demo/dof/tof_3.dof

    qsyn> verb 0
    Note: verbose level is set to 0

    qsyn> zxgread benchmark/zx/tof3.zx

    qsyn> zxgs -freduce

    qsyn> zxgp
    Graph 0( 3 inputs, 3 outputs, 17 vertices, 19 edges )

    qsyn> q -f
    ```



### Testing
There are two types of testing approaches:
1. DOFILEs, which automatically run a sequence of commands. (Located under `tests/<section>/<subsection>/dof/`)

    * To run a DOFILE and compare the result to the reference, type
		```shell!
		./DOFILE.sh <path/to/test> -d
		```
    * To update the reference to a dofile, type
		```shell!
		./DOFILE.sh <path/to/test> -up
		```
    * You may also run all DOFILEs by running
        ```bash!
        ./RUN_ALL_TEST.sh
        ```
2. Unit tests, which checks the validity of selected data structures.
	
    * To compile unit test programs, type
		```shell!
		make test -j16
		```
	* Then, run the test by 
		```shell!
		./qsyn-test -r compact
		```


## License
[Apache License 2.0](https://github.com/ric2k1/qsyn/blob/main/LICENSE)
## Commands List

### Info
| Command      | Description                         			| Options     |
| --------     | --------                            			| --------    |
| COLOR        | toggle colored printing (1: on, 0: off)  		|             |
| DOfile       | execute the commands in the dofile  			|             |
| HELp         | print this help message             			|             |
| HIStory      | print command history               			|             |
| QQuit        | quit Qsyn                  			        |             |
| SEED         | fix the seed                        			|             |
| USAGE        | report the runtime and/or memory usage         |             |
| VERbose      | set verbose level to 0-9 (default: 3)          |             |



### QCir

| Command      | Description                         					| Options        |
| --------     | --------                            					| --------       |
| QC2TS         | convert QCir to tensor                                 |                |
| QC2ZX         | convert QCir to ZX-graph                               |                |
| QCBAdd        | add qubit(s)                                           |                |
| QCBDelete     | delete an empty qubit                                  |                |
| QCCHeckout    | checkout to QCir <id> in QCirMgr                       |                |
| QCCOMpose     | compose a QCir                                         |                |
| QCCOPy        | copy a QCir                                            |                |
| QCCPrint      | print info of QCir                                     |                |
| QCCRead       | read a circuit and construct the corresponding netlist |                |
| QCCWrite      | write QCir to a QASM file                              |                |
| QCDelete      | remove a QCir from QCirMgr                             |                |
| QCGAdd        | add quantum gate                                       |                |
| QCGDelete     | delete quantum gate                                    |                |
| QCGPrint      | print gate info in QCir                                |                |
| QCNew         | create a new QCir to QCirMgr                           |                |
| QCPrint       | print info of QCirMgr                                  |                |
| QCReset       | reset QCirMgr                                          |                |
| QCTensor      | tensor a QCir                                          |                |



### Graph

| Command       | Description                         								| Options     |
| --------      | --------                            								| --------    |
| ZX2QC         | extract QCir from ZX-graph                                        |             |
| ZX2TS         | convert ZX-graph to tensor                                        |             |
| ZXCHeckout    | checkout to Graph <id> in ZXGraphMgr                				|             |
| ZXCOMpose     | compose a ZX-graph				                                |             |
| ZXCOPy        | copy a ZX-graph				                                    |             |
| ZXDelete      | remove a ZX-graph from ZXGraphMgr                                 |             |
| ZXGADJoint    | adjoint ZX-graph                                                  |             |
| ZXGASsign     | assign quantum states to input/output vertex					    |             |
| ZXGDraw       | draw ZX-graph                                                     |             |
| ZXGEdit       | edit ZX-graph    			                                        |             |
| ZXGGFlow      | calculate the generalized flow of current ZX-graph                |             |
| ZXGPrint      | print info of ZX-graph    			                            |             |
| ZXGRead       | read a file and construct the corresponding ZX-graph    			|             |
| ZXGSimp       | perform simplification strategies for ZX-graph        			|             |
| ZXGTest       | test ZX-graph structures and functions	    			        |             |
| ZXGTRaverse   | traverse ZX-graph and update topological order of vertices	    |             |
| ZXGWrite      | write a ZX-graph to a file    			                        |             |
| ZXNew         | create a new ZX-graph to ZXGraphMgr        			            |             |
| ZXPrint       | print info of ZXGraphMgr	    			                        |             |
| ZXReset       | reset ZXGraphMgr                                                  |             |
| ZXTensor      | tensor a ZX-graph				                                    |             |



### Tensor
| Command      | Description                                         | Options     |
| --------     | --------                                            | --------    |
| TSADJoint    | adjoint the specified tensor                        |             |
| TSEQuiv      | check the equivalency of two stored tensors         |             |
| TSPrint      | print info of stored tensors                        |             |
| TSReset      | reset the tensor manager                            |             |

### Extraction
| Command      | Description                                         | Options     |
| --------     | --------                                            | --------    |    
| EXTPrint     | print info of extracting ZX-graph                   |             |
| EXTRact      | perform step(s) in extraction                       |             |

### Lattice
| Command      | Description                                                                     | Options     |
| --------     | --------                                                                        | --------    |
| LTS          | (experimental) perform mapping from ZX-graph to corresponding lattice surgery   |             |