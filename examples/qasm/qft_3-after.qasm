OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
h q[2];
h q[1];
h q[1];
cz q[1],q[0];
h q[0];
cz q[0],q[2];
cz q[1],q[2];
h q[2];
cz q[1],q[0];
tdg q[0];
h q[0];
cz q[2],q[1];
cz q[0],q[1];
rz(5*pi/4) q[1];
h q[1];
tdg q[1];
h q[1];
rz(15*pi/8) q[2];
h q[2];
cx q[2],q[0];
h q[0];
rz(3*pi/8) q[0];
cx q[1],q[2];
h q[2];
rz(3*pi/8) q[2];
h q[2];
h q[1];
t q[1];
