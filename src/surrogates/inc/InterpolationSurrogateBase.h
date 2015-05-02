//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
//
// QUESO - a library to support the Quantification of Uncertainty
// for Estimation, Simulation and Optimization
//
// Copyright (C) 2008-2015 The PECOS Development Team
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Version 2.1 GNU Lesser General
// Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------el-

#ifndef UQ_INTERPOLATION_SURROGATE_BASE_H
#define UQ_INTERPOLATION_SURROGATE_BASE_H

#include <queso/SurrogateBase.h>
#include <queso/BoxSubset.h>
#include <queso/VectorSpace.h>
#include <queso/InterpolationSurrogateData.h>

namespace QUESO
{
  //! Base class for interpolation-based surrogates
  /*! This class is used for surrogoate approximations of a model using interpolation.
      Subclasses will define behavior of interpolant, but common assumptions
      are:
           -# Bounded domain; future work may extend behavior to unbounded domains.
           -# Structured grids on the parameter domain. Subclasses will determine behavior
              of spacing (uniform vs. directionally-uniform, etc.).

      For the structured grid, we think of referencing each "node" in the box by its
      index coordinates (i,j,k,...), where i runs from (0, n_points[0]-1),
      j run from (0,n_points[1]-1), etc. We use this indexing to build maps. */
  template<class V, class M>
  class InterpolationSurrogateBase : public SurrogateBase<V>
  {
  public:

    //! Constructor
    /*! n_points should be a vector with dimension matching the dimension of
        the parameter space and contain the number of points in each coordinate
        direction for the interpolant. */
    InterpolationSurrogateBase(const InterpolationSurrogateData<V,M>& data);

    virtual ~InterpolationSurrogateBase(){};

  protected:

    const InterpolationSurrogateData<V,M>& m_data;

  private:

    InterpolationSurrogateBase();

  };

} // end namespace QUESO

#endif // UQ_INTERPOLATION_SURROGATE_BASE_H
