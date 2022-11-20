/*
 *  This code is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This code is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this code; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* Copyright (C) 2022 Max-Planck-Society
   Author: Martin Reinecke */


#ifndef DUCC0_ARRAY_DESCRIPTOR_H
#define DUCC0_ARRAY_DESCRIPTOR_H

#include <array>
#include "ducc0/infra/error_handling.h"
#include "ducc0/infra/mav.h"
#include "ducc0/bindings/typecode.h"

namespace ducc0 {

namespace detail_array_descriptor {

using namespace std;

struct ArrayDescriptor
  {
  static constexpr size_t maxdim=10;

  array<uint64_t, maxdim> shape;
  array<int64_t, maxdim> stride;

  void *data;
  uint8_t ndim;
  uint8_t dtype;
  };
template<typename T, size_t ndim> auto prep1(const ArrayDescriptor &desc)
  {
  static_assert(ndim<=ArrayDescriptor::maxdim, "dimensionality too high");
  MR_assert(ndim==desc.ndim, "dimensionality mismatch");
  MR_assert(Typecode<T>::value==desc.dtype, "data type mismatch");
  typename mav_info<ndim>::shape_t shp;
  typename mav_info<ndim>::stride_t str;
  for (size_t i=0; i<ndim; ++i)
    {
    shp[i] = desc.shape[ndim-1-i];
    str[i] = desc.stride[ndim-1-i];
    }
  return make_tuple(shp, str);
  }
template<typename T, size_t ndim> cmav<T,ndim> to_cmav(const ArrayDescriptor &desc)
  {
  auto [shp, str] = prep1<T, ndim>(desc);
  return cmav<T, ndim>(reinterpret_cast<const T *>(desc.data), shp, str);
  }
template<typename T, size_t ndim> vmav<T,ndim> to_vmav(ArrayDescriptor &desc)
  {
  auto [shp, str] = prep1<T, ndim>(desc);
  return vmav<T, ndim>(reinterpret_cast<T *>(desc.data), shp, str);
  }
template<typename T> auto prep2(const ArrayDescriptor &desc)
  {
  MR_assert(Typecode<T>::value==desc.dtype, "data type mismatch");
  typename fmav_info::shape_t shp(desc.ndim);
  typename fmav_info::stride_t str(desc.ndim);
  for (size_t i=0; i<desc.ndim; ++i)
    {
    shp[i] = desc.shape[desc.ndim-1-i];
    str[i] = desc.stride[desc.ndim-1-i];
    }
  return make_tuple(shp, str);
  }
template<typename T> cfmav<T> to_cfmav(const ArrayDescriptor &desc)
  {
  auto [shp, str] = prep2<T>(desc);
  return cfmav<T>(reinterpret_cast<const T *>(desc.data), shp, str);
  }
template<typename T> vfmav<T> to_vfmav(ArrayDescriptor &desc)
  {
  auto [shp, str] = prep2<T>(desc);
  return vfmav<T>(reinterpret_cast<T *>(desc.data), shp, str);
  }

}

using detail_array_descriptor::ArrayDescriptor;
using detail_array_descriptor::to_cmav;
using detail_array_descriptor::to_vmav;
using detail_array_descriptor::to_cfmav;
using detail_array_descriptor::to_vfmav;

}

#endif
