#pragma once
#include <functional>
#include <vector>

#include <cereal/access.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

#include "gncpy/math/Vector.h"
#include "gncpy/math/Matrix.h"
#include "gncpy/math/Math.h"
#include "gncpy/measurements/Parameters.h"
#include "gncpy/measurements/IMeasModel.h"
#include "gncpy/SerializeMacros.h"


namespace lager::gncpy::measurements{

template<typename T>
class INonLinearMeasModel : public IMeasModel<T> {

friend class cereal::access;

public:
    matrix::Vector<T> measure(const matrix::Vector<T>& state, const MeasParams* params=nullptr) const override {
        std::vector<T> data;
        for (auto const& h : this->getMeasFuncLst(params)) {
            data.emplace_back(h(state));
        }
        return matrix::Vector<T>(data.size(), data);
    }
    matrix::Matrix<T> getMeasMat(const matrix::Vector<T>& state, const MeasParams* params=nullptr) const override {
        return math::getJacobian(state, this->getMeasFuncLst(params));
    };
protected:
    virtual std::vector<std::function<T (const matrix::Vector<T>&)>> getMeasFuncLst(const MeasParams* params=nullptr) const = 0; 

private:
    template <class Archive>
    void serialize([[maybe_unused]] Archive& ar) {
        ar(cereal::make_nvp("IMeasModel", cereal::virtual_base_class<IMeasModel<T>>(this)));
    }
};
} // namespace lager::gncpy::measurements

GNCPY_SERIALIZE_TYPES(lager::gncpy::measurements::INonLinearMeasModel)
