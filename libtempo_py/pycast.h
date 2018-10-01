//
// Created by egordm on 1-10-2018.
//

#ifndef PROJECT_PYCAST_H
#define PROJECT_PYCAST_H

#include <armadillo>
#include <pybind11/cast.h>
#include <pybind11/numpy.h>
//#include <pybind11/eigen.h>

#define MAT_T arma::Mat<T>

namespace pybind11 {
    namespace detail {

        template<typename MT>
        struct ArmaProps {
            using Type = MT;
            using Scalar = typename Type::elem_type;

            static PYBIND11_DESCR descriptor() {
                return type_descr(_("numpy.ndarray[") + npy_format_descriptor<Scalar>::name() + _("]"));
            }
        };

        /**
         * Casts an Arma type to numpy array.If given a base, the numpy array references the src data,
         * otherwise it'll make a copy.
         * @tparam T
         * @param src
         * @param base
         * @param writeable: lets you turn off the writeable flag for the array
         * @return
         */
        template<typename MT>
        handle arma_array_cast(const MT &src, handle base = handle(), bool writeable = true) {
            constexpr ssize_t elem_size = sizeof(typename ArmaProps<MT>::Scalar);

            array a;
            if (src.n_cols > 1) {
                a = array({(ssize_t) src.n_rows, (ssize_t) src.n_cols},
                          {(ssize_t) (elem_size), (ssize_t) (elem_size * src.n_rows)},
                          src.memptr(),
                          base);
            } else {
                a = array({(ssize_t) src.n_rows}, {elem_size}, src.memptr(), base);
            }

            if (!writeable) array_proxy(a.ptr())->flags &= ~detail::npy_api::NPY_ARRAY_WRITEABLE_;

            return a.release();
        }

        /**
         * Takes an lvalue ref to some Eigen type and a (python) base object, creating a numpy array that
         * reference the Armadillo object's data with `base` as the python-registered base class (if omitted,
         * the base will be set to None, and lifetime management is up to the caller).  The numpy array is
         * non-writeable if the given type is const.
         * @tparam MT
         * @param src
         * @param parent
         * @return
         */
        template<typename MT>
        handle arma_ref_array(MT &src, handle parent = none()) {
            // none here is to get past array's should-we-copy detection, which currently always
            // copies when there is no base.  Setting the base to None should be harmless.
            return arma_array_cast<MT>(src, parent, !std::is_const<MT>::value); // Check for const is useless
        }

        /**
         * Takes a pointer to some Armadillo type, builds a capsule around it, then returns a numpy
         * array that references the encapsulated data with a python-side reference to the capsule to tie
         * its destruction to that of any dependent python objects. Const-ness is determined by whether or
         * not the Type of the pointer given is const.
         * @tparam T
         * @param src
         * @return
         */
        template<typename MT>
        handle arma_encapsulate(MT *src) {
            capsule base(src, [](void *o) { delete static_cast<MT *>(o); });
            return arma_ref_array<MT>(*src, base);
        }

        template<typename T>
        struct type_caster<arma::Mat<T>> {
            using Type = MAT_T;
            using Props = ArmaProps<Type>;

        PYBIND11_TYPE_CASTER(Type, Props::descriptor());

            /**
             * Converts python numpy array into an armadillo matrix
             * @param src
             * @param convert
             * @return
             */
            bool load(handle src, bool convert) {
                // If we're in no-convert mode, only load if given an array of the correct type
                if (!convert && !isinstance<array_t<T>>(src)) return false;

                // Coerce into an array, but don't do type conversion yet; the copy below handles it.
                auto buf = array::ensure(src);
                if (!buf) return false;

                auto dims = buf.ndim();
                if (dims < 1 || dims > 2) return false;

                // Allocate the new type, then build a numpy reference into it
                if (dims == 1) value = Type((const uword) buf.shape(0), 1);
                else value = Type((const uword) buf.shape(0), (const uword) buf.shape(1));

                auto ref = reinterpret_steal<array>(arma_ref_array<Type>(value));

                int result = detail::npy_api::get().PyArray_CopyInto_(ref.ptr(), buf.ptr());

                if (result < 0) { // Copy failed!
                    PyErr_Clear();
                    return false;
                }

                return true;
            }

            template<typename CT>
            static handle cast_impl(CT *src, return_value_policy policy, handle parent) {
                switch (policy) {
                    case return_value_policy::take_ownership:
                    case return_value_policy::automatic:
                        return arma_encapsulate<CT>(src);
                    case return_value_policy::move:
                        return arma_encapsulate<CT>(new CT(std::move(*src)));
                    case return_value_policy::copy:
                        return arma_array_cast<CT>(*src);
                    case return_value_policy::reference:
                    case return_value_policy::automatic_reference:
                        return arma_ref_array<CT>(*src);
                    case return_value_policy::reference_internal:
                        return arma_ref_array<CT>(*src, parent);
                    default:
                        throw cast_error("unhandled return_value_policy: should not happen!");
                };
            }

            // Normal returned non-reference, non-const value:
            static handle cast(Type &&src, return_value_policy /* policy */, handle parent) {
                return cast_impl(&src, return_value_policy::move, parent);
            }

            // If you return a non-reference const, we mark the numpy array readonly:
            static handle cast(const Type &&src, return_value_policy /* policy */, handle parent) {
                return cast_impl(&src, return_value_policy::move, parent);
            }

            // lvalue reference return; default (automatic) becomes copy
            static handle cast(Type &src, return_value_policy policy, handle parent) {
                if (policy == return_value_policy::automatic || policy == return_value_policy::automatic_reference)
                    policy = return_value_policy::copy;
                return cast_impl(&src, policy, parent);
            }

            // const lvalue reference return; default (automatic) becomes copy
            static handle cast(const Type &src, return_value_policy policy, handle parent) {
                if (policy == return_value_policy::automatic || policy == return_value_policy::automatic_reference)
                    policy = return_value_policy::copy;
                return cast(&src, policy, parent);
            }

            // non-const pointer return
            static handle cast(Type *src, return_value_policy policy, handle parent) {
                return cast_impl(src, policy, parent);
            }

            // const pointer return
            static handle cast(const Type *src, return_value_policy policy, handle parent) {
                return cast_impl(src, policy, parent);
            }
        };
    }
}

#endif //PROJECT_PYCAST_H
