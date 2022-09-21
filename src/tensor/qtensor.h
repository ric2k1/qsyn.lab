/****************************************************************************
  FileName     [ qtensor.h ]
  PackageName  [ tensor ]
  Synopsis     [ Definition of the QTensor class ]
  Author       [ Mu-Te (Joshua) Lau ]
  Copyright    [ 2022 9 ]
****************************************************************************/
#ifndef TENSOR_H
#define TENSOR_H

#include "tensor.h"
#include "phase.h"

template <typename T>
class QTensor : public Tensor<std::complex<T>>{
protected:
    using DataType = Tensor<std::complex<T>>::DataType;
    using InternalType = Tensor<std::complex<T>>::InternalType;
public:
    QTensor(const Tensor<DataType>& t) : Tensor<DataType>(t) {}
    QTensor(Tensor<DataType>&& t) : Tensor<DataType>(t) {}

    QTensor(xt::nested_initializer_list_t<DataType, 0> il): Tensor<DataType>(il) {}
    QTensor(xt::nested_initializer_list_t<DataType, 1> il): Tensor<DataType>(il) {}
    QTensor(xt::nested_initializer_list_t<DataType, 2> il): Tensor<DataType>(il) {}
    QTensor(xt::nested_initializer_list_t<DataType, 3> il): Tensor<DataType>(il) {}
    QTensor(xt::nested_initializer_list_t<DataType, 4> il): Tensor<DataType>(il) {}
    QTensor(xt::nested_initializer_list_t<DataType, 5> il): Tensor<DataType>(il) {}

    QTensor(const TensorShape& shape) : Tensor<DataType>(shape) {}
    QTensor(TensorShape&& shape) : Tensor<DataType>(shape) {}
    template <typename From>
    requires std::convertible_to<From, InternalType>
    QTensor(const From& internal) : Tensor<DataType>(internal) {}
    template <typename From>
    requires std::convertible_to<From, InternalType>
    QTensor(From&& internal) : Tensor<DataType>(internal) {}

    static QTensor<T> identity(const size_t& n);
    static QTensor<T> zspider(const size_t& n, const Phase& phase = Phase(0));
    static QTensor<T> xspider(const size_t& n, const Phase& phase = Phase(0));
    static QTensor<T> hbox(const size_t& n, const DataType& a = -1.);
    static QTensor<T> rz(const Phase& phase = Phase(0));
    static QTensor<T> rx(const Phase& phase = Phase(0));
    static QTensor<T> cnz(const size_t& n);
    static QTensor<T> cnx(const size_t& n);

    QTensor<T> selfTensordot(const TensorAxisList& ax1 = {}, const TensorAxisList& ax2 = {});


private:
    static DataType nuPow(const int& n);
};

//------------------------------
// tensor builders functions
//------------------------------

// Generate an tensor that corresponds to a n-qubit identity gate.
// Note that `n` is the number of qubits, not dimensions
template<typename T>
QTensor<T> QTensor<T>::identity(const size_t& n) {
    InternalType t = xt::eye<DataType>(intPow(2, n));
    t.reshape(TensorShape(2*n, 2));
    return t;
}

// Generate an tensor that corresponds to a n-qubit Z-spider.
template<typename T>
QTensor<T> QTensor<T>::zspider(const size_t& n, const Phase& phase) {
    QTensor<T> t = xt::zeros<DataType>(TensorShape(n, 2));
    if (n == 0) {
        t() = 1. + std::exp(1.0i * phase.toFloatType<T>());
    } else {
        t[TensorIndex(n, 0)] = 1.;
        t[TensorIndex(n, 1)] = std::exp(1.0i * phase.toFloatType<T>());
    }
    t._tensor *= nuPow(2 - n);
    return t;
}

// Generate an tensor that corresponds to a n-qubit X-spider.
template<typename T>
QTensor<T> QTensor<T>::xspider(const size_t& n, const Phase& phase) {
    QTensor<T> t = xt::ones<QTensor<T>::DataType>(TensorShape(n, 2));
    QTensor<T> ketMinus(TensorShape{2});
    ketMinus(0, 0) = 1.;
    ketMinus(0, 1) = -1.;
    QTensor<T> tmp = tensorPow(ketMinus, n);
    t._tensor += tmp._tensor * std::exp(1.0i * phase.toFloatType<T>());
    t._tensor /= std::pow(std::sqrt(2), n);
    t._tensor *= nuPow(2 - n);
    return t;
}

// Generate an tensor that corresponds to a n-qubit H-box.
// The t(1, ..., 1) element is set to be `exp(i*phase)`
template<typename T>
QTensor<T> QTensor<T>::hbox(const size_t& n, const QTensor<T>::DataType& a) {
    QTensor<T> t = xt::ones<QTensor<T>::DataType>(TensorShape(n, 2));
    if (n == 0) {
        t() = a;
    } else {
        t[TensorIndex(n, 1)] = a;
    }
    t._tensor *= nuPow(n);
    return t; 
}

// Generate an tensor that corresponds to a Rz gate.
// Axis order: <in, out>
template<typename T>
QTensor<T> QTensor<T>::rz(const Phase& phase) {
    return QTensor<T>::zspider(2, phase);
}

// Generate an tensor that corresponds to a Rx gate.
// Axis order: <in, out>
template<typename T>
QTensor<T> QTensor<T>::rx(const Phase& phase) {
    return QTensor<T>::xspider(2, phase);
}
// Generate an tensor that corresponds to a n-controlled Rz gate.
// Axis order: [c1-in, c1-out, ..., cn-in, cn-out], <t-in, t-out>
template<typename T>
QTensor<T> QTensor<T>::cnz(const size_t& n) {
    if (n == 0) {
        return QTensor<T>::rz(Phase(1));
    } else {
        QTensor<T> t = QTensor<T>::hbox(n + 1);
        for (size_t i = 0; i <= n; ++i) {
            t = tensordot(t, QTensor<T>::zspider(3), {0}, {0});
        }
        return t;
    }
}

// Generate an tensor that corresponds to a n-controlled Rx gate.
// Axis order: [c1-in, c1-out, ..., cn-in, cn-out], <t-in, t-out>
template<typename T>
QTensor<T> QTensor<T>::cnx(const size_t& n) {
    if (n == 0) {
        return QTensor<T>::rx(Phase(1));
    } else {
        QTensor<T> t = QTensor<T>::hbox(n + 1);
        t = tensordot(t, QTensor<T>::hbox(2), {n}, {1});
        for (size_t i = 0; i < n; ++i) {
            t = tensordot(t, QTensor<T>::zspider(3), {0}, {0});
        }
        t = tensordot(t, QTensor<T>::xspider(3), {0}, {0});
        return t;
    }
}

//------------------------------
// tensor manipulation functions
//------------------------------

// tensor-dot a tensor between pairs of axes
// dots the tensor  along the axes in ax1 and ax2
template<typename T>
QTensor<T> QTensor<T>::selfTensordot(const TensorAxisList& ax1, const TensorAxisList& ax2) {
    if (ax1.size() != ax2.size()) {
        throw std::invalid_argument("The two index orders should contain the same number of indices.");
    }
    if (!isDisjoint(ax1, ax2)) {
        throw std::invalid_argument("The two index orders should be disjoint.");
    }
    if (ax1.empty()) return *this;
    auto tmp = QTensor<T>::identity(ax1.size());
    auto tmpOrder = TensorAxisList(ax1.size() + ax2.size(), 0);
    std::iota(tmpOrder.begin(), tmpOrder.end(), 0);
    QTensor<T> u = tensordot(*this, tmp, concatAxisList(ax1, ax2), tmpOrder);
    u._tensor *= nuPow(2 * ax1.size());
    return u;
}

//------------------------------
// Private member functions
//------------------------------

// Calculate (2^(1/4))^n
template<typename T>
QTensor<T>::DataType QTensor<T>::nuPow(const int& n) {
    return std::pow(2., -0.25 * n);
}

#endif  // TENSOR_H