OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
rz(pi/2) q[0];
sx q[0];
rz(1.4282941) q[0];
rz(2.3145031) q[1];
sx q[1];
rz(pi/2) q[1];
cx q[0],q[1];
rz(1.7132985) q[0];
sx q[0];
rz(pi/2) q[0];
rz(pi/2) q[1];
sx q[1];
rz(0.82708959) q[1];
