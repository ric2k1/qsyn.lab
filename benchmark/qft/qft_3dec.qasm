OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
h q[0];
p(pi/4) q[1];
p(pi/4) q[0];
cx q[1],q[0];
p(-pi/4) q[0];
cx q[1],q[0];
p(pi/8) q[2];
p(pi/8) q[0];
cx q[2],q[0];
p(-pi/8) q[0];
cx q[2],q[0];
h q[1];
p(pi/4) q[2];
p(pi/4) q[1];
cx q[2],q[1];
p(-pi/4) q[1];
cx q[2],q[1];
h q[2];
