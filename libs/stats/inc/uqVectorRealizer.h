/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------
 *
 * Copyright (C) 2008 The PECOS Development Team
 *
 * Please see http://pecos.ices.utexas.edu for more information.
 *
 * This file is part of the QUESO Library (Quantification of Uncertainty
 * for Estimation, Simulation and Optimization).
 *
 * QUESO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QUESO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QUESO. If not, see <http://www.gnu.org/licenses/>.
 *
 *--------------------------------------------------------------------------
 *
 * $Id$
 *
 * Brief description of this file: 
 * 
 *--------------------------------------------------------------------------
 *-------------------------------------------------------------------------- */

#ifndef __UQ_REALIZER_H__
#define __UQ_REALIZER_H__

#include <uqVectorSequence.h>
#include <uqEnvironment.h>
#include <math.h>

//*****************************************************
// Classes to accomodate a probability density routine.
//*****************************************************

//*****************************************************
// Base class
//*****************************************************
template<class V, class M>
class uqBaseVectorRealizerClass {
public:
           uqBaseVectorRealizerClass(const char*                  prefix,
                                     const uqVectorSetClass<V,M>& unifiedImageSet,
                                     unsigned int                 subPeriod);

  virtual ~uqBaseVectorRealizerClass();

  const   uqVectorSetClass<V,M>& unifiedImageSet()              const;
          unsigned int           subPeriod      ()              const;
  virtual void                   realization    (V& nextValues) const = 0;

protected:
  const uqBaseEnvironmentClass& m_env;
        std::string             m_prefix;
  const uqVectorSetClass<V,M>&  m_unifiedImageSet;
        unsigned int            m_subPeriod;
};

template<class V, class M>
uqBaseVectorRealizerClass<V,M>::uqBaseVectorRealizerClass(
  const char*                  prefix,
  const uqVectorSetClass<V,M>& unifiedImageSet,
  unsigned int                 subPeriod)
  :
  m_env            (unifiedImageSet.env()),
  m_prefix         ((std::string)(prefix)+"re_"),
  m_unifiedImageSet(unifiedImageSet),
  m_subPeriod      (subPeriod)
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqBaseVectorRealizerClass<V,M>::constructor() [4]"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqBaseVectorRealizerClass<V,M>::constructor() [4]"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}

template<class V, class M>
uqBaseVectorRealizerClass<V,M>::~uqBaseVectorRealizerClass()
{
}

template<class V, class M>
unsigned int
uqBaseVectorRealizerClass<V,M>::subPeriod() const
{
  return m_subPeriod;
}

template<class V, class M>
const uqVectorSetClass<V,M>&
uqBaseVectorRealizerClass<V,M>::unifiedImageSet() const
{
  return m_unifiedImageSet;
}

//*****************************************************
// Generic class
//*****************************************************
template<class V, class M>
class uqGenericVectorRealizerClass : public uqBaseVectorRealizerClass<V,M> {
public:
  uqGenericVectorRealizerClass(const char*                  prefix,
                               const uqVectorSetClass<V,M>& unifiedImageSet,
                               unsigned int                 subPeriod,
                               double (*routinePtr)(const void* routineDataPtr, V& nextParamValues),
                               const void* routineDataPtr);
 ~uqGenericVectorRealizerClass();

  void realization(V& nextValues) const;

private:
  double (*m_routinePtr)(const void* routineDataPtr, V& nextParamValues);
  const void* m_routineDataPtr;

  using uqBaseVectorRealizerClass<V,M>::m_env;
  using uqBaseVectorRealizerClass<V,M>::m_prefix;
  using uqBaseVectorRealizerClass<V,M>::m_unifiedImageSet;
  using uqBaseVectorRealizerClass<V,M>::m_subPeriod;
};

template<class V, class M>
uqGenericVectorRealizerClass<V,M>::uqGenericVectorRealizerClass(
  const char*                  prefix,
  const uqVectorSetClass<V,M>& unifiedImageSet,
  unsigned int                 subPeriod,
  double (*routinePtr)(const void* routineDataPtr, V& nextParamValues),
  const void* routineDataPtr)
  :
  uqBaseVectorRealizerClass<V,M>(((std::string)(prefix)+"gen").c_str(),unifiedImageSet,subPeriod),
  m_routinePtr    (routinePtr),
  m_routineDataPtr(routineDataPtr)
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqGenericVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqGenericVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}

template<class V, class M>
uqGenericVectorRealizerClass<V,M>::~uqGenericVectorRealizerClass()
{
}

template<class V, class M>
void
uqGenericVectorRealizerClass<V,M>::realization(V& nextValues) const
{
  m_routinePtr(m_routineDataPtr,nextValues);
  return;
}

//*****************************************************
// Gaussian class
//*****************************************************
template<class V, class M>
class uqGaussianVectorRealizerClass : public uqBaseVectorRealizerClass<V,M> {
public:
  uqGaussianVectorRealizerClass(const char*                  prefix,
                                const uqVectorSetClass<V,M>& unifiedImageSet,
                                const V&                     lawExpVector, // vector of mean values
                                const M&                     lowerCholLawCovMatrix); // lower triangular matrix resulting from Cholesky decomposition of the covariance matrix

  uqGaussianVectorRealizerClass(const char*                  prefix,
                                const uqVectorSetClass<V,M>& unifiedImageSet,
                                const V&                     lawExpVector, // vector of mean values
                                const M&                     matU,
                                const V&                     vecSsqrt,
                                const M&                     matVt);

  ~uqGaussianVectorRealizerClass();

  const V&   unifiedLawExpVector        ()              const;
  const V&   unifiedLawVarVector        ()              const;
        void realization                (V& nextValues) const;
        void updateLawExpVector         (const V& newLawExpVector);
        void updateLowerCholLawCovMatrix(const M& newLowerCholLawCovMatrix);
        void updateLowerCholLawCovMatrix(const M& matU,
                                         const V& vecSsqrt,
                                         const M& matVt);
    
private:
  V* m_unifiedLawExpVector;
  V* m_unifiedLawVarVector;
  M* m_lowerCholLawCovMatrix;
  M* m_matU;
  V* m_vecSsqrt;
  M* m_matVt;

  using uqBaseVectorRealizerClass<V,M>::m_env;
  using uqBaseVectorRealizerClass<V,M>::m_prefix;
  using uqBaseVectorRealizerClass<V,M>::m_unifiedImageSet;
  using uqBaseVectorRealizerClass<V,M>::m_subPeriod;
};

template<class V, class M>
uqGaussianVectorRealizerClass<V,M>::uqGaussianVectorRealizerClass(const char* prefix,
								  const uqVectorSetClass<V,M>& unifiedImageSet,
								  const V& lawExpVector,
								  const M& lowerCholLawCovMatrix)
  :
  uqBaseVectorRealizerClass<V,M>( ((std::string)(prefix)+"gau").c_str(), unifiedImageSet, 0 ),
  m_unifiedLawExpVector  (new V(lawExpVector)),
  m_unifiedLawVarVector  (unifiedImageSet.vectorSpace().newVector( INFINITY)), // FIX ME
  m_lowerCholLawCovMatrix(new M(lowerCholLawCovMatrix)),
  m_matU                 (NULL),
  m_vecSsqrt             (NULL),
  m_matVt                (NULL)
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqGaussianVectorRealizerClass<V,M>::constructor() [1]"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  *m_unifiedLawExpVector = lawExpVector; // ????

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqGaussianVectorRealizerClass<V,M>::constructor() [1]"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}
								  
template<class V, class M>
uqGaussianVectorRealizerClass<V,M>::uqGaussianVectorRealizerClass(const char* prefix,
								  const uqVectorSetClass<V,M>& unifiedImageSet,
								  const V& lawExpVector,
								  const M& matU,
								  const V& vecSsqrt,
								  const M& matVt)
  :
  uqBaseVectorRealizerClass<V,M>( ((std::string)(prefix)+"gau").c_str(), unifiedImageSet, 0 ),
  m_unifiedLawExpVector  (new V(lawExpVector)),
  m_unifiedLawVarVector  (unifiedImageSet.vectorSpace().newVector( INFINITY)), // FIX ME
  m_lowerCholLawCovMatrix(NULL),
  m_matU                 (new M(matU)),
  m_vecSsqrt             (new V(vecSsqrt)),
  m_matVt                (new M(matVt))
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqGaussianVectorRealizerClass<V,M>::constructor() [2]"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  *m_unifiedLawExpVector = lawExpVector; // ????

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqGaussianVectorRealizerClass<V,M>::constructor() [2]"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}
								  
template<class V, class M>
uqGaussianVectorRealizerClass<V,M>::~uqGaussianVectorRealizerClass()
{
  delete m_matVt;
  delete m_vecSsqrt;
  delete m_matU;
  delete m_lowerCholLawCovMatrix;
  delete m_unifiedLawVarVector;
  delete m_unifiedLawExpVector;
}

template <class V, class M>
const V&
uqGaussianVectorRealizerClass<V,M>::unifiedLawExpVector() const
{
  return *m_unifiedLawExpVector;
}

template <class V, class M>
const V&
uqGaussianVectorRealizerClass<V,M>::unifiedLawVarVector() const
{
  return *m_unifiedLawVarVector;
}

template<class V, class M>
void
uqGaussianVectorRealizerClass<V,M>::realization(V& nextValues) const
{
  V iidGaussianVector(m_unifiedImageSet.vectorSpace().zeroVector());
  iidGaussianVector.cwSetGaussian(m_env.rng(), 0.0, 1.0);

  if (m_lowerCholLawCovMatrix) {
    nextValues = (*m_unifiedLawExpVector) + (*m_lowerCholLawCovMatrix)*iidGaussianVector;
  }
  else if (m_matU && m_vecSsqrt && m_matVt) {
    nextValues = (*m_unifiedLawExpVector) + (*m_matU)*( (*m_vecSsqrt) * ((*m_matVt)*iidGaussianVector) );
  }
  else {
    UQ_FATAL_TEST_MACRO(true,
                        m_env.fullRank(),
                        "uqGaussianVectorRealizerClass<V,M>::realization()",
                        "inconsistent internal state");
  }

  return;
}

template<class V, class M>
void
uqGaussianVectorRealizerClass<V,M>::updateLawExpVector(const V& newLawExpVector)
{
  // delete old expected values (alloced at construction or last call to this function)
  delete m_unifiedLawExpVector;

  m_unifiedLawExpVector = new V(newLawExpVector);
 
  return;
}

template<class V, class M>
void
uqGaussianVectorRealizerClass<V,M>::updateLowerCholLawCovMatrix(const M& newLowerCholLawCovMatrix)
{
  // delete old expected values (alloced at construction or last call to this function)
  delete m_lowerCholLawCovMatrix;
  delete m_matU;
  delete m_vecSsqrt;
  delete m_matVt;

  m_lowerCholLawCovMatrix = new M(newLowerCholLawCovMatrix);
  m_matU                  = NULL;
  m_vecSsqrt              = NULL;
  m_matVt                 = NULL;

  return;
}

template<class V, class M>
void
uqGaussianVectorRealizerClass<V,M>::updateLowerCholLawCovMatrix(
  const M& matU,
  const V& vecSsqrt,
  const M& matVt)
{
  // delete old expected values (alloced at construction or last call to this function)
  delete m_lowerCholLawCovMatrix;
  delete m_matU;
  delete m_vecSsqrt;
  delete m_matVt;

  m_lowerCholLawCovMatrix = NULL;
  m_matU                  = new M(matU);
  m_vecSsqrt              = new V(vecSsqrt);
  m_matVt                 = new M(matVt);

  return;
}

//*****************************************************
// Sequential class
//*****************************************************
template<class V, class M>
class uqSequentialVectorRealizerClass : public uqBaseVectorRealizerClass<V,M> {
public:
  uqSequentialVectorRealizerClass(const char*                           prefix,
                                  const uqBaseVectorSequenceClass<V,M>& chain);
 ~uqSequentialVectorRealizerClass();

  const V&   unifiedSampleExpVector()              const;
  const V&   unifiedSampleVarVector()              const;
        void realization           (V& nextValues) const;

private:
  const   uqBaseVectorSequenceClass<V,M>& m_chain;
  mutable unsigned int                    m_currentChainPos;
          V*                              m_unifiedSampleExpVector;
          V*                              m_unifiedSampleVarVector;

  using uqBaseVectorRealizerClass<V,M>::m_env;
  using uqBaseVectorRealizerClass<V,M>::m_prefix;
  using uqBaseVectorRealizerClass<V,M>::m_unifiedImageSet;
  using uqBaseVectorRealizerClass<V,M>::m_subPeriod;
};

template<class V, class M>
uqSequentialVectorRealizerClass<V,M>::uqSequentialVectorRealizerClass(
  const char*                           prefix,
  const uqBaseVectorSequenceClass<V,M>& chain)
  :
  uqBaseVectorRealizerClass<V,M>(((std::string)(prefix)+"seq").c_str(),chain.unifiedValuesBox(),chain.subSequenceSize()),
  m_chain                 (chain),
  m_currentChainPos       (0),
  m_unifiedSampleExpVector(new V(chain.unifiedMeanValues()          )), // IMPORTANT
  m_unifiedSampleVarVector(new V(chain.unifiedSampleVarianceValues()))  // IMPORTANT
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 0)) {
    *m_env.subDisplayFile() << "In uqSequentialVectorRealizerClass<V,M>::constructor()"
                            << ": m_chain.subSequenceSize() = " << m_chain.subSequenceSize()
                            << std::endl;
  }
}

template<class V, class M>
uqSequentialVectorRealizerClass<V,M>::~uqSequentialVectorRealizerClass()
{
  delete m_unifiedSampleVarVector;
  delete m_unifiedSampleExpVector;
}

template<class V, class M>
void
uqSequentialVectorRealizerClass<V,M>::realization(V& nextParamValues) const
{
  m_chain.getPositionValues(m_currentChainPos++,nextParamValues);
  if (m_currentChainPos >= m_subPeriod) m_currentChainPos = 0;

  return;
}

template <class V, class M>
const V&
uqSequentialVectorRealizerClass<V,M>::unifiedSampleExpVector() const
{
  return *m_unifiedSampleExpVector;
}

template <class V, class M>
const V&
uqSequentialVectorRealizerClass<V,M>::unifiedSampleVarVector() const
{
  return *m_unifiedSampleVarVector;
}

//*****************************************************
// Uniform class
//*****************************************************
template<class V, class M>
class uqUniformVectorRealizerClass : public uqBaseVectorRealizerClass<V,M> {
public:
  uqUniformVectorRealizerClass(const char*                  prefix,
                               const uqVectorSetClass<V,M>& unifiedImageSet);
 ~uqUniformVectorRealizerClass();

  void realization(V& nextValues) const;

private:
  using uqBaseVectorRealizerClass<V,M>::m_env;
  using uqBaseVectorRealizerClass<V,M>::m_prefix;
  using uqBaseVectorRealizerClass<V,M>::m_unifiedImageSet;
  using uqBaseVectorRealizerClass<V,M>::m_subPeriod;
};

template<class V, class M>
uqUniformVectorRealizerClass<V,M>::uqUniformVectorRealizerClass(
  const char*                  prefix,
  const uqVectorSetClass<V,M>& unifiedImageSet)
  :
  uqBaseVectorRealizerClass<V,M>(((std::string)(prefix)+"gen").c_str(),unifiedImageSet,std::numeric_limits<unsigned int>::max())
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqUniformVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqUniformVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}

template<class V, class M>
uqUniformVectorRealizerClass<V,M>::~uqUniformVectorRealizerClass()
{
}

template<class V, class M>
void
uqUniformVectorRealizerClass<V,M>::realization(V& nextValues) const
{
  const uqBoxSubsetClass<V,M>* imageBox = dynamic_cast<const uqBoxSubsetClass<V,M>* >(&m_unifiedImageSet);

  UQ_FATAL_TEST_MACRO(imageBox == NULL,
                      m_env.fullRank(),
                      "uqUniformVectorRealizerClass<V,M>::realization()",
                      "only box images are supported right now");
  
  nextValues.cwSetUniform(m_env.rng(),imageBox->minValues(),imageBox->maxValues());
  return;
}

//*****************************************************
// InverseGamma class
//*****************************************************
template<class V, class M>
class uqInverseGammaVectorRealizerClass : public uqBaseVectorRealizerClass<V,M> {
public:
  uqInverseGammaVectorRealizerClass(const char*                  prefix,
                                    const uqVectorSetClass<V,M>& unifiedImageSet,
                                    const V&                     alpha,
                                    const V&                     beta);
 ~uqInverseGammaVectorRealizerClass();

  void realization(V& nextValues) const;

private:
  using uqBaseVectorRealizerClass<V,M>::m_env;
  using uqBaseVectorRealizerClass<V,M>::m_prefix;
  using uqBaseVectorRealizerClass<V,M>::m_unifiedImageSet;
  using uqBaseVectorRealizerClass<V,M>::m_subPeriod;

  V m_alpha;
  V m_beta;
};

template<class V, class M>
uqInverseGammaVectorRealizerClass<V,M>::uqInverseGammaVectorRealizerClass(
  const char*                  prefix,
  const uqVectorSetClass<V,M>& unifiedImageSet,
  const V&                     alpha,
  const V&                     beta)
  :
  uqBaseVectorRealizerClass<V,M>(((std::string)(prefix)+"gen").c_str(),unifiedImageSet,std::numeric_limits<unsigned int>::max()),
  m_alpha(alpha),
  m_beta (beta)
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqInverseGammaVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqInverseGammaVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}

template<class V, class M>
uqInverseGammaVectorRealizerClass<V,M>::~uqInverseGammaVectorRealizerClass()
{
}

template<class V, class M>
void
uqInverseGammaVectorRealizerClass<V,M>::realization(V& nextValues) const
{
  nextValues.cwSetInverseGamma(m_env.rng(),m_alpha,m_beta);
  return;
}

//*****************************************************
// Concatenated class
//*****************************************************
template<class V, class M>
class uqConcatenatedVectorRealizerClass : public uqBaseVectorRealizerClass<V,M> {
public:
  uqConcatenatedVectorRealizerClass(const char*                           prefix,
                                    const uqBaseVectorRealizerClass<V,M>& realizer1,
                                    const uqBaseVectorRealizerClass<V,M>& realizer2,
                                    const uqVectorSetClass<V,M>&          unifiedImageSet);
 ~uqConcatenatedVectorRealizerClass();

  void realization(V& nextValues) const;

private:
  using uqBaseVectorRealizerClass<V,M>::m_env;
  using uqBaseVectorRealizerClass<V,M>::m_prefix;
  using uqBaseVectorRealizerClass<V,M>::m_unifiedImageSet;
  using uqBaseVectorRealizerClass<V,M>::m_subPeriod;

  const uqBaseVectorRealizerClass<V,M>& m_realizer1;
  const uqBaseVectorRealizerClass<V,M>& m_realizer2;
};

template<class V, class M>
uqConcatenatedVectorRealizerClass<V,M>::uqConcatenatedVectorRealizerClass(
  const char*                           prefix,
  const uqBaseVectorRealizerClass<V,M>& realizer1,
  const uqBaseVectorRealizerClass<V,M>& realizer2,
  const uqVectorSetClass<V,M>&          unifiedImageSet)
  :
  uqBaseVectorRealizerClass<V,M>(((std::string)(prefix)+"gen").c_str(),unifiedImageSet,std::numeric_limits<unsigned int>::max()),
  m_realizer1(realizer1),
  m_realizer2(realizer2)
{
  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Entering uqConcatenatedVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }

  if ((m_env.subDisplayFile()) && (m_env.displayVerbosity() >= 5)) {
    *m_env.subDisplayFile() << "Leaving uqConcatenatedVectorRealizerClass<V,M>::constructor()"
                            << ": prefix = " << m_prefix
                            << std::endl;
  }
}

template<class V, class M>
uqConcatenatedVectorRealizerClass<V,M>::~uqConcatenatedVectorRealizerClass()
{
}

template<class V, class M>
void
uqConcatenatedVectorRealizerClass<V,M>::realization(V& nextValues) const
{
  V v1(m_realizer1.unifiedImageSet().vectorSpace().zeroVector());
  V v2(m_realizer2.unifiedImageSet().vectorSpace().zeroVector());

  //std::cout << "In uqConcatenatedVectorRealizerClass<V,M>::realization: v1.sizeLocal() = " << v1.sizeLocal() << std::endl;
  m_realizer1.realization(v1);
  //std::cout << "In uqConcatenatedVectorRealizerClass<V,M>::realization: v2.sizeLocal() = " << v2.sizeLocal() << std::endl;
  m_realizer2.realization(v2);

  //std::cout << "In uqConcatenatedVectorRealizerClass<V,M>::realization: nextValues.sizeLocal() = " << nextValues.sizeLocal() << std::endl;
  nextValues.cwSetConcatenated(v1,v2);
  //std::cout << "In uqConcatenatedVectorRealizerClass<V,M>::realization: succeeded" << std::endl;

  return;
}
#endif // __UQ_REALIZER_H__
