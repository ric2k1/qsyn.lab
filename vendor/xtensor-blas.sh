ENGINE_NAME="xtensor-blas"
TIMESTAMP=$(date -I)
VENDOR_PATH="/tmp/qsyn-configure-"$TIMESTAMP
GITHUB_PATH="https://github.com/xtensor-stack/xtensor-blas.git"
JOB=16
if [ $# -ne 1 ]; then 
    echo "[Error] Missing install path!"
    exit 1
fi

INSTALL_PATH=$1

mkdir -p ${VENDOR_PATH}
rm -rf ${VENDOR_PATH}/${ENGINE_NAME}
cd ${VENDOR_PATH}
git clone ${GITHUB_PATH} ${ENGINE_NAME}

cd ${ENGINE_NAME}

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH}
make -j${JOB}
sudo make install