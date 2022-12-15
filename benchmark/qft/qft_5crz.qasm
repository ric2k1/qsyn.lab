OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
h q[0];
rz(pi/2) q[1];
crz(pi/2) q[1],q[0];
rz(pi/4) q[2];
crz(pi/4) q[2],q[0];
rz(pi/8) q[3];
crz(pi/8) q[3],q[0];
rz(pi/16) q[4];
crz(pi/16) q[4],q[0];
h q[1];
rz(pi/2) q[2];
crz(pi/2) q[2],q[1];
rz(pi/4) q[3];
crz(pi/4) q[3],q[1];
rz(pi/8) q[4];
crz(pi/8) q[4],q[1];
h q[2];
rz(pi/2) q[3];
crz(pi/2) q[3],q[2];
rz(pi/4) q[4];
crz(pi/4) q[4],q[2];
h q[3];
rz(pi/2) q[4];
crz(pi/2) q[4],q[3];
h q[4];