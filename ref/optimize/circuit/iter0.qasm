OPENQASM 2.0;
include "qelib1.inc";
qreg q[20];
h q[7];
h q[2];
cz q[1],q[2];
cx q[1],q[3];
p(5*pi/4) q[1];
cz q[0],q[1];
h q[0];
cz q[0],q[2];
cx q[0],q[3];
p(31*pi/16) q[3];
cz q[0],q[1];
cx q[1],q[2];
h q[2];
p(15*pi/8) q[2];
cx q[1],q[2];
p(7*pi/4) q[0];
cx q[0],q[2];
cx q[2],q[4];
h q[4];
p(11*pi/8) q[2];
cx q[1],q[0];
cx q[0],q[3];
cx q[0],q[12];
p(12995*pi/6498) q[12];
h q[1];
cz q[1],q[2];
p(7*pi/4) q[1];
cz q[1],q[4];
cx q[2],q[1];
cx q[1],q[3];
p(15*pi/8) q[3];
cx q[1],q[5];
h q[5];
cx q[1],q[13];
p(12995*pi/6498) q[13];
cx q[0],q[2];
cx q[2],q[4];
h q[4];
cx q[5],q[2];
h q[2];
p(15*pi/8) q[2];
cx q[3],q[2];
cx q[1],q[3];
cz q[4],q[5];
p(31*pi/16) q[4];
cx q[4],q[2];
cx q[2],q[6];
h q[6];
cz q[3],q[6];
p(7*pi/16) q[3];
p(3*pi/4) q[2];
cz q[2],q[3];
cx q[1],q[4];
cx q[0],q[4];
p(63*pi/32) q[4];
cx q[0],q[5];
h q[5];
cz q[5],q[6];
cx q[6],q[4];
p(31*pi/16) q[5];
cz q[2],q[5];
cx q[4],q[5];
cx q[6],q[4];
cx q[2],q[3];
cx q[3],q[5];
cx q[3],q[8];
p(255*pi/128) q[8];
cx q[3],q[14];
p(12995*pi/6498) q[14];
cx q[2],q[5];
cx q[6],q[2];
cx q[2],q[5];
cx q[1],q[2];
cx q[3],q[6];
cx q[1],q[6];
h q[6];
cx q[3],q[8];
cx q[0],q[5];
h q[5];
cx q[0],q[2];
h q[2];
cz q[2],q[5];
p(15*pi/8) q[2];
cx q[1],q[2];
cx q[1],q[8];
cx q[0],q[4];
cz q[4],q[7];
cz q[6],q[7];
p(31*pi/16) q[6];
cx q[6],q[5];
cx q[7],q[5];
p(15*pi/32) q[4];
cx q[4],q[5];
h q[5];
p(3*pi/4) q[5];
cx q[7],q[4];
cx q[4],q[2];
cx q[7],q[5];
cx q[5],q[6];
cx q[4],q[6];
cx q[5],q[4];
cx q[4],q[9];
p(255*pi/128) q[9];
cx q[4],q[15];
p(12995*pi/6498) q[15];
cx q[5],q[2];
p(63*pi/32) q[2];
cx q[1],q[2];
cx q[3],q[7];
cx q[3],q[5];
cx q[1],q[5];
cz q[1],q[9];
cz q[1],q[15];
cx q[0],q[6];
p(255*pi/128) q[6];
cx q[0],q[2];
p(127*pi/64) q[2];
cx q[0],q[7];
h q[7];
p(31*pi/16) q[7];
cx q[6],q[7];
cx q[1],q[7];
cx q[3],q[6];
cx q[0],q[5];
h q[5];
p(15*pi/8) q[5];
cz q[0],q[9];
cx q[4],q[9];
cx q[0],q[7];
h q[7];
cz q[5],q[7];
cx q[0],q[6];
p(63*pi/32) q[6];
cx q[6],q[5];
cx q[3],q[5];
cx q[2],q[5];
cx q[2],q[7];
h q[7];
p(255*pi/128) q[7];
cx q[7],q[8];
h q[8];
cx q[4],q[7];
cx q[3],q[6];
cx q[1],q[6];
p(127*pi/64) q[6];
cx q[1],q[7];
p(63*pi/32) q[7];
cx q[0],q[9];
p(1983*pi/992) q[9];
cx q[0],q[5];
cx q[5],q[10];
p(3*pi/4) q[5];
cx q[0],q[2];
cx q[2],q[10];
p(255*pi/128) q[10];
cx q[2],q[5];
cz q[5],q[10];
cz q[4],q[10];
cz q[3],q[10];
cz q[5],q[13];
cx q[5],q[16];
p(12995*pi/6498) q[16];
cz q[3],q[16];
p(95*pi/64) q[2];
h q[2];
p(15*pi/8) q[2];
cz q[2],q[8];
h q[8];
p(31*pi/16) q[8];
cx q[7],q[8];
cx q[5],q[8];
cx q[4],q[8];
cx q[5],q[10];
cx q[2],q[8];
p(63*pi/32) q[8];
cx q[7],q[2];
cx q[6],q[2];
cx q[4],q[2];
cx q[1],q[2];
cz q[2],q[8];
cx q[2],q[11];
p(3*pi/4) q[2];
cx q[1],q[10];
p(1983*pi/992) q[10];
cx q[1],q[6];
cz q[6],q[8];
cz q[6],q[9];
cx q[8],q[9];
cx q[5],q[8];
cx q[5],q[9];
cx q[6],q[11];
p(255*pi/128) q[11];
cz q[5],q[11];
p(191*pi/128) q[6];
cx q[6],q[2];
cz q[2],q[13];
cz q[2],q[14];
cx q[2],q[17];
p(12995*pi/6498) q[17];
cx q[5],q[6];
cx q[4],q[7];
cx q[3],q[7];
p(127*pi/64) q[7];
cx q[3],q[7];
cx q[4],q[10];
cx q[1],q[10];
cx q[4],q[8];
p(127*pi/64) q[8];
cz q[4],q[11];
cz q[3],q[11];
cz q[2],q[11];
cz q[1],q[11];
cx q[1],q[6];
cx q[3],q[11];
cx q[2],q[11];
p(1983*pi/992) q[11];
cz q[4],q[17];
cx q[0],q[7];
p(507*pi/254) q[7];
cx q[0],q[9];
h q[9];
cx q[0],q[6];
h q[6];
cz q[6],q[9];
h q[9];
p(15*pi/8) q[9];
cx q[8],q[9];
cx q[9],q[10];
p(63*pi/32) q[10];
p(31*pi/16) q[6];
cx q[4],q[6];
cx q[2],q[6];
cx q[9],q[6];
p(63*pi/32) q[6];
cx q[7],q[9];
cx q[4],q[8];
cx q[1],q[8];
p(507*pi/254) q[8];
cx q[1],q[8];
cx q[4],q[9];
cx q[0],q[8];
p(1007*pi/504) q[8];
cx q[0],q[9];
cz q[9],q[12];
cz q[9],q[10];
cz q[9],q[11];
cx q[6],q[11];
p(3*pi/4) q[9];
cx q[0],q[7];
cz q[7],q[12];
cz q[7],q[10];
p(24321*pi/16256) q[7];
cx q[9],q[7];
cz q[7],q[14];
cx q[7],q[18];
p(12995*pi/6498) q[18];
p(3*pi/7) q[7];
cx q[7],q[12];
cx q[7],q[10];
cx q[6],q[10];
cx q[3],q[10];
cx q[11],q[10];
h q[10];
cx q[5],q[6];
cx q[2],q[6];
p(127*pi/64) q[6];
cz q[5],q[18];
cx q[3],q[9];
cx q[1],q[9];
h q[9];
cz q[9],q[10];
h q[10];
p(31*pi/16) q[10];
cx q[5],q[10];
p(63*pi/32) q[9];
cx q[9],q[11];
cx q[3],q[11];
cx q[2],q[11];
p(15*pi/8) q[11];
cx q[6],q[11];
cx q[2],q[10];
cx q[11],q[10];
p(127*pi/64) q[10];
cx q[0],q[8];
p(510127*pi/1024128) q[8];
cx q[0],q[12];
p(255*pi/128) q[12];
cx q[7],q[12];
cx q[4],q[12];
p(1983*pi/992) q[12];
cx q[4],q[12];
cx q[2],q[12];
cx q[10],q[12];
cx q[7],q[9];
cx q[5],q[9];
cx q[11],q[9];
p(127*pi/64) q[9];
cx q[10],q[9];
cx q[8],q[11];
cx q[5],q[6];
cx q[3],q[6];
p(507*pi/254) q[6];
cx q[3],q[6];
cx q[1],q[6];
p(1007*pi/504) q[6];
cx q[1],q[6];
p(15845941*pi/31747968) q[6];
cx q[5],q[11];
p(7*pi/4) q[11];
cz q[10],q[11];
cx q[11],q[8];
cz q[8],q[13];
cx q[8],q[19];
p(12995*pi/6498) q[19];
p(198388856*pi/396724907) q[8];
cx q[8],q[13];
cx q[1],q[13];
p(255*pi/128) q[13];
cz q[8],q[19];
cx q[8],q[13];
cx q[4],q[10];
cx q[7],q[9];
cx q[2],q[9];
h q[9];
cx q[2],q[10];
p(507*pi/254) q[10];
cx q[7],q[11];
cx q[4],q[11];
cx q[3],q[11];
h q[11];
cx q[11],q[12];
p(63*pi/32) q[12];
p(15*pi/8) q[11];
cz q[9],q[11];
h q[9];
p(31*pi/16) q[9];
cx q[10],q[9];
cx q[11],q[9];
cx q[5],q[9];
cx q[5],q[13];
p(1983*pi/992) q[13];
cx q[11],q[12];
cx q[10],q[12];
cx q[10],q[11];
cx q[6],q[11];
cx q[4],q[9];
p(507*pi/254) q[9];
cz q[6],q[9];
cz q[7],q[19];
cz q[2],q[19];
cx q[8],q[19];
cx q[8],q[12];
cx q[4],q[12];
p(127*pi/64) q[12];
cx q[9],q[12];
cx q[5],q[9];
cx q[4],q[11];
p(7*pi/4) q[11];
cx q[6],q[11];
cz q[11],q[14];
cz q[11],q[18];
cx q[11],q[14];
cx q[4],q[10];
cx q[3],q[10];
p(1007*pi/504) q[10];
cx q[3],q[10];
cx q[3],q[14];
p(255*pi/128) q[14];
cx q[11],q[14];
cx q[4],q[9];
p(1007*pi/504) q[9];
cx q[8],q[12];
cx q[5],q[12];
h q[12];
cx q[5],q[6];
cx q[4],q[6];
h q[6];
cz q[6],q[12];
h q[12];
p(31*pi/16) q[12];
cx q[9],q[12];
p(15*pi/8) q[6];
cx q[6],q[12];
cx q[9],q[6];
cx q[7],q[18];
cx q[0],q[10];
p(3845*pi/1923) q[10];
cx q[0],q[10];
cz q[10],q[13];
p(0*pi) q[10];
cx q[10],q[6];
cx q[4],q[6];
p(7*pi/4) q[6];
cx q[4],q[12];
cx q[2],q[12];
p(507*pi/254) q[12];
cx q[13],q[12];
cx q[5],q[12];
cx q[2],q[12];
h q[12];
cx q[2],q[14];
p(1983*pi/992) q[14];
cx q[4],q[9];
cx q[1],q[9];
p(3845*pi/1923) q[9];
cx q[1],q[9];
cx q[10],q[6];
cz q[6],q[15];
cx q[6],q[13];
cx q[5],q[13];
p(63*pi/32) q[13];
cx q[6],q[15];
cx q[4],q[15];
p(255*pi/128) q[15];
cx q[7],q[15];
cx q[6],q[15];
p(1983*pi/992) q[15];
cx q[5],q[10];
cx q[2],q[10];
h q[10];
cz q[10],q[12];
h q[12];
p(15*pi/8) q[12];
cx q[13],q[12];
p(31*pi/16) q[10];
cx q[10],q[12];
cx q[13],q[10];
cx q[11],q[13];
cx q[0],q[9];
p(7247*pi/3624) q[9];
cx q[0],q[9];
cx q[9],q[10];
cx q[6],q[10];
p(7*pi/4) q[10];
cx q[6],q[12];
cx q[5],q[12];
p(1007*pi/504) q[12];
cx q[5],q[12];
cx q[3],q[12];
p(3845*pi/1923) q[12];
cx q[3],q[12];
cx q[1],q[12];
cz q[12],q[15];
p(7247*pi/3624) q[12];
cx q[6],q[13];
p(127*pi/64) q[13];
cx q[11],q[13];
cz q[9],q[14];
cx q[2],q[14];
p(5860101*pi/11722250) q[9];
cx q[9],q[10];
cx q[10],q[14];
p(63*pi/32) q[14];
cz q[10],q[16];
cx q[10],q[19];
p(1983*pi/992) q[19];
p(15845941*pi/31747968) q[10];
cx q[7],q[9];
cx q[2],q[9];
h q[9];
p(31*pi/16) q[9];
cx q[10],q[9];
cx q[10],q[16];
cx q[5],q[16];
p(255*pi/128) q[16];
cx q[7],q[13];
p(507*pi/254) q[13];
cx q[7],q[9];
cx q[13],q[9];
cx q[14],q[9];
p(15*pi/8) q[9];
cx q[12],q[9];
cx q[2],q[9];
cx q[1],q[9];
cx q[1],q[12];
cx q[10],q[14];
cx q[6],q[14];
p(127*pi/64) q[14];
cx q[10],q[16];
cx q[8],q[16];
p(1983*pi/992) q[16];
cx q[8],q[14];
cx q[6],q[14];
p(507*pi/254) q[14];
cx q[7],q[13];
cx q[2],q[13];
p(1007*pi/504) q[13];
cx q[13],q[9];
cx q[4],q[13];
cx q[2],q[13];
p(3845*pi/1923) q[13];
cx q[4],q[13];
cx q[3],q[13];
cz q[13],q[16];
p(7247*pi/3624) q[13];
cx q[7],q[15];
cx q[12],q[15];
cx q[9],q[15];
p(63*pi/32) q[15];
p(7*pi/4) q[9];
p(pi/3) q[12];
cx q[12],q[9];
cz q[9],q[17];
p(510127*pi/1024128) q[9];
h q[12];
p(31*pi/16) q[12];
cx q[14],q[12];
cx q[15],q[12];
cx q[9],q[12];
cx q[8],q[12];
p(15*pi/8) q[12];
cx q[9],q[17];
cx q[2],q[17];
p(255*pi/128) q[17];
cx q[11],q[17];
cx q[10],q[15];
cx q[9],q[15];
p(127*pi/64) q[15];
cx q[11],q[15];
cx q[9],q[17];
p(1983*pi/992) q[17];
cx q[10],q[15];
p(507*pi/254) q[15];
cx q[8],q[14];
cx q[7],q[14];
p(1007*pi/504) q[14];
cx q[7],q[12];
cx q[3],q[12];
cx q[13],q[12];
cx q[14],q[12];
p(7*pi/4) q[12];
cx q[12],q[16];
cx q[8],q[16];
cx q[3],q[13];
cx q[13],q[16];
p(63*pi/32) q[16];
p(214930*pi/429723) q[13];
cx q[7],q[14];
cx q[5],q[14];
p(3845*pi/1923) q[14];
cx q[5],q[14];
cx q[4],q[14];
cz q[14],q[17];
p(7247*pi/3624) q[14];
cx q[13],q[0];
p(21537*pi/10769) q[0];
cx q[12],q[0];
cx q[13],q[12];
cx q[12],q[18];
p(255*pi/128) q[18];
cx q[12],q[0];
p(3*pi/7) q[0];
h q[13];
p(31*pi/16) q[13];
cx q[15],q[13];
cx q[16],q[13];
cx q[12],q[13];
cx q[11],q[13];
p(15*pi/8) q[13];
cx q[14],q[13];
cx q[11],q[17];
cx q[12],q[18];
cx q[6],q[18];
p(1983*pi/992) q[18];
cx q[12],q[16];
cx q[9],q[16];
p(127*pi/64) q[16];
cx q[9],q[16];
cx q[6],q[16];
p(507*pi/254) q[16];
cx q[11],q[15];
cx q[8],q[15];
p(1007*pi/504) q[15];
cx q[8],q[13];
cx q[4],q[13];
cx q[15],q[13];
cx q[13],q[17];
p(7*pi/4) q[13];
cx q[4],q[14];
cx q[14],q[17];
p(63*pi/32) q[17];
p(343888*pi/687557) q[14];
cx q[14],q[1];
p(21537*pi/10769) q[1];
cx q[13],q[1];
cx q[14],q[13];
cx q[13],q[1];
p(3*pi/7) q[1];
h q[14];
p(31*pi/16) q[14];
cx q[16],q[14];
cx q[17],q[14];
cx q[13],q[14];
cx q[6],q[14];
p(15*pi/8) q[14];
cx q[13],q[17];
cx q[12],q[17];
p(127*pi/64) q[17];
cx q[12],q[17];
cx q[10],q[17];
p(507*pi/254) q[17];
cx q[8],q[15];
cx q[2],q[15];
p(3845*pi/1923) q[15];
cx q[5],q[15];
cx q[2],q[15];
cz q[15],q[18];
p(7247*pi/3624) q[15];
cx q[15],q[14];
cx q[11],q[16];
cx q[6],q[16];
p(1007*pi/504) q[16];
cx q[6],q[18];
cx q[11],q[14];
cx q[5],q[14];
cx q[16],q[14];
cx q[14],q[18];
p(7*pi/4) q[14];
cx q[5],q[15];
cx q[15],q[18];
p(63*pi/32) q[18];
p(1791*pi/3581) q[15];
cx q[15],q[3];
p(21537*pi/10769) q[3];
cx q[14],q[3];
cx q[15],q[14];
p(31*pi/64) q[14];
cx q[14],q[3];
p(3*pi/7) q[3];
h q[15];
p(31*pi/16) q[15];
cx q[17],q[15];
cx q[18],q[15];
cx q[14],q[15];
cx q[10],q[15];
p(15*pi/8) q[15];
cx q[14],q[18];
cx q[13],q[18];
p(127*pi/64) q[18];
cx q[13],q[18];
cx q[9],q[18];
p(507*pi/254) q[18];
cx q[10],q[17];
cx q[6],q[17];
p(1007*pi/504) q[17];
cx q[11],q[16];
cx q[7],q[16];
p(3845*pi/1923) q[16];
cx q[7],q[16];
cx q[2],q[16];
cz q[16],q[19];
cx q[14],q[19];
cx q[10],q[19];
p(127*pi/64) q[19];
cx q[14],q[19];
cx q[9],q[19];
p(1007*pi/504) q[19];
p(7247*pi/3624) q[16];
cx q[16],q[15];
cx q[6],q[15];
cx q[2],q[15];
cx q[17],q[15];
p(7*pi/4) q[15];
cx q[8],q[17];
cx q[6],q[17];
p(3845*pi/1923) q[17];
cx q[8],q[17];
cx q[2],q[16];
p(pi/2) q[16];
cx q[16],q[4];
p(21537*pi/10769) q[4];
cx q[15],q[4];
cx q[16],q[15];
cx q[15],q[4];
p(3*pi/7) q[4];
h q[16];
p(31*pi/16) q[16];
cx q[18],q[16];
cx q[7],q[17];
cz q[17],q[19];
cx q[19],q[16];
p(15*pi/8) q[16];
p(7247*pi/3624) q[17];
cx q[17],q[16];
cx q[10],q[16];
cx q[10],q[18];
cx q[9],q[18];
p(1007*pi/504) q[18];
cx q[7],q[16];
cx q[18],q[16];
p(7*pi/4) q[16];
cx q[10],q[18];
cx q[8],q[18];
cx q[7],q[17];
p(1791*pi/3581) q[17];
cx q[17],q[5];
p(21537*pi/10769) q[5];
cx q[16],q[5];
cx q[17],q[16];
p(7*pi/16) q[16];
cx q[16],q[5];
p(3*pi/7) q[5];
h q[17];
p(15*pi/8) q[17];
cx q[11],q[19];
cx q[9],q[19];
p(7247*pi/3624) q[19];
cx q[19],q[15];
cx q[19],q[13];
cx q[19],q[12];
cx q[19],q[6];
cz q[18],q[19];
cx q[19],q[17];
p(3623*pi/3624) q[18];
cx q[8],q[18];
cx q[18],q[19];
p(5372*pi/3581) q[18];
cx q[18],q[15];
cx q[18],q[13];
cx q[18],q[12];
cx q[18],q[11];
cx q[11],q[15];
p(63*pi/32) q[15];
cx q[11],q[13];
p(255*pi/128) q[13];
cx q[11],q[12];
p(507*pi/254) q[12];
cx q[11],q[19];
cx q[7],q[19];
p(21537*pi/10769) q[19];
cx q[7],q[19];
p(40939*pi/81852) q[19];
cx q[19],q[15];
p(15*pi/32) q[15];
cx q[19],q[13];
p(63*pi/128) q[13];
cx q[19],q[12];
p(8065*pi/16256) q[12];
p(3845*pi/1923) q[11];
cx q[11],q[6];
cx q[17],q[11];
p(7*pi/4) q[11];
cx q[18],q[6];
p(3845*pi/1923) q[6];
cx q[19],q[6];
p(pi/2) q[6];
cx q[6],q[7];
cx q[7],q[6];
cx q[6],q[7];
cx q[7],q[8];
cx q[8],q[7];
cx q[7],q[8];
cx q[18],q[2];
p(21537*pi/10769) q[2];
cx q[11],q[2];
cx q[18],q[11];
cx q[11],q[17];
p(4292*pi/8585) q[17];
cx q[8],q[17];
cx q[17],q[8];
cx q[8],q[17];
cx q[9],q[17];
cx q[17],q[9];
cx q[9],q[17];
p(3*pi/8) q[11];
cx q[11],q[2];
p(3*pi/7) q[2];
cx q[2],q[3];
cx q[3],q[2];
cx q[2],q[3];
cx q[3],q[4];
cx q[4],q[3];
cx q[3],q[4];
cx q[4],q[5];
cx q[5],q[4];
cx q[4],q[5];
cx q[11],q[17];
cx q[17],q[11];
cx q[11],q[17];
h q[18];
p(7*pi/4) q[18];
cx q[19],q[18];
p(pi/4) q[18];
h q[19];
