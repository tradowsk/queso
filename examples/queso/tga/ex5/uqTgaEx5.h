/* uq/examples/queso/tga/uqTgaEx5.h
 *
 * Copyright (C) 2008 The QUESO Team, http://queso.ices.utexas.edu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __UQ_TGA_EX5_H__
#define __UQ_TGA_EX5_H__

#include <uqCalibProblem.h>
#include <uqPropagProblem.h>
#include <uqAsciiTable.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h> 
#define R_CONSTANT 8.314472

// The ODE (state dot) function
int func(double t, const double Mass[], double f[], void *info)
{
  double* params = (double *)info;
  double A    = params[0];
  double E    = params[1];
  double beta = params[2];

  f[0] = -A*Mass[0]*exp(-E/(R_CONSTANT*t))/beta;

  return GSL_SUCCESS;
}

//********************************************************
// Likelihood function object for the first validation problem stage (with prefix "s1_").
// A likelihood function object is provided by user and is called by the UQ library.
// This likelihood function object consists of data and routine.
//********************************************************

// The (user defined) data type for the data needed by the (user defined) likelihood routine
template<class P_V, class P_M>
struct
calibLikelihoodRoutine_DataType
{
  double               beta;
  double               variance;
  std::vector<double>* Te; // temperatures
  std::vector<double>* Me; // relative masses
};

// The actual (user defined) likelihood routine
template<class P_V,class P_M>
double
calibLikelihoodRoutine(const P_V& paramValues, const void* functionDataPtr)
{
  double A                       = paramValues[0];
  double E                       = paramValues[1];
  double beta                    =  ((calibLikelihoodRoutine_DataType<P_V,P_M> *) functionDataPtr)->beta;
  double variance                =  ((calibLikelihoodRoutine_DataType<P_V,P_M> *) functionDataPtr)->variance;
  const std::vector<double>& Te  = *((calibLikelihoodRoutine_DataType<P_V,P_M> *) functionDataPtr)->Te;
  const std::vector<double>& Me  = *((calibLikelihoodRoutine_DataType<P_V,P_M> *) functionDataPtr)->Me;
  std::vector<double> Mt(Me.size(),0.);

  double params[]={A,E,beta};
      	
  // integration
  const gsl_odeiv_step_type *T   = gsl_odeiv_step_rkf45; //rkf45; //gear1;
        gsl_odeiv_step      *s   = gsl_odeiv_step_alloc(T,1);
        gsl_odeiv_control   *c   = gsl_odeiv_control_y_new(1e-6,0.0);
        gsl_odeiv_evolve    *e   = gsl_odeiv_evolve_alloc(1);
        gsl_odeiv_system     sys = {func, NULL, 1, (void *)params}; 
	
  double t = 0.1, t1 = 1900.;
  double h = 1e-3;
  double Mass[1];
  Mass[0]=1.;
  
  unsigned int i = 0;
  double t_old = 0.;
  double M_old[1];
  M_old[0]=1.;
	
  double misfit=0.;
  //unsigned int loopSize = 0;
  while ((t < t1) && (i < Me.size())) {
    int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &t, t1, &h, Mass);
    UQ_FATAL_TEST_MACRO((status != GSL_SUCCESS),
                        paramValues.env().rank(),
                        "calibLikelihoodRoutine()",
                        "gsl_odeiv_evolve_apply() failed");
    //printf("t = %6.1lf, mass = %10.4lf\n",t,Mass[0]);
    //loopSize++;
		
    while ( (i < Me.size()) && (t_old <= Te[i]) && (Te[i] <= t) ) {
      Mt[i] = (Te[i]-t_old)*(Mass[0]-M_old[0])/(t-t_old) + M_old[0];
      misfit += (Me[i]-Mt[i])*(Me[i]-Mt[i]);
      //printf("%i %lf %lf %lf %lf\n",i,Te[i],Me[i],Mt[i],misfit);
      i++;
    }
		
    t_old=t;
    M_old[0]=Mass[0];
  }
  double resultValue = misfit/variance;
	
  //printf("loopSize = %d\n",loopSize);
  if ((paramValues.env().verbosity() >= 10) && (paramValues.env().rank() == 0)) {
    printf("In calibLikelihoodRoutine(), A = %g, E = %g, beta = %.3lf: misfit = %lf, likelihood = %lf.\n",A,E,beta,misfit,resultValue);
  }

  gsl_odeiv_evolve_free (e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free   (s);

  return resultValue;
}

//********************************************************
// QoI function object for the first validation problem stage (with prefix "s1_").
// A QoI function object is provided by user and is called by the UQ library.
// This QoI function object consists of data and routine.
//********************************************************
// The (user defined) data type for the data needed by the (user defined) qoi routine
template<class P_V,class P_M,class Q_V, class Q_M>
struct
propagQoiRoutine_DataType
{
  double beta;
  double criticalMass;
};

// The actual (user defined) qoi routine
template<class P_V,class P_M,class Q_V,class Q_M>
void propagQoiRoutine(const P_V& paramValues, const void* functionDataPtr, Q_V& qoiValues)
{
  double A             = paramValues[0];
  double E             = paramValues[1];
  double beta          = ((propagQoiRoutine_DataType<P_V,P_M,Q_V,Q_M> *) functionDataPtr)->beta;
  double criticalMass  = ((propagQoiRoutine_DataType<P_V,P_M,Q_V,Q_M> *) functionDataPtr)->criticalMass;

  double params[]={A,E,beta};
      	
  // integration
  const gsl_odeiv_step_type *T   = gsl_odeiv_step_rkf45; //rkf45; //gear1;
        gsl_odeiv_step      *s   = gsl_odeiv_step_alloc(T,1);
        gsl_odeiv_control   *c   = gsl_odeiv_control_y_new(1e-6,0.0);
        gsl_odeiv_evolve    *e   = gsl_odeiv_evolve_alloc(1);
        gsl_odeiv_system     sys = {func, NULL, 1, (void *)params}; 
	
  double t = 0.1, t1 = 1100.;
  double h = 1e-3;
  double Mass[1];
  Mass[0]=1.;
  
  double t_old = 0.;
  double M_old[1];
  M_old[0]=1.;
	
  double crossingTemperature = 0.;
  //unsigned int loopSize = 0;
  while ((t       < t1          ) &&
         (Mass[0] > criticalMass)) {
    int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &t, t1, &h, Mass);
    UQ_FATAL_TEST_MACRO((status != GSL_SUCCESS),
                        paramValues.env().rank(),
                        "propagQoiRoutine()",
                        "gsl_odeiv_evolve_apply() failed");
    //printf("t = %6.1lf, mass = %10.4lf\n",t,Mass[0]);
    //loopSize++;

    if (Mass[0] <= criticalMass) {
      crossingTemperature = t_old + (t - t_old) * (M_old[0]-criticalMass)/(M_old[0]-Mass[0]);
    }
		
    t_old=t;
    M_old[0]=Mass[0];
  }

  qoiValues[0] = crossingTemperature/beta;
	
  //printf("loopSize = %d\n",loopSize);
  if ((paramValues.env().verbosity() >= 10) && (paramValues.env().rank() == 0)) {
    printf("In propagQoiRoutine(), A = %g, E = %g, beta = %.3lf, criticalMass = %.3lf: qoi = %lf.\n",A,E,beta,criticalMass,qoiValues[0]);
  }

  gsl_odeiv_evolve_free (e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free   (s);

  return;
}

//********************************************************
// The driving routine "uqAppl()": called by main()
// Step   I.1 of 3: code very user specific to stage I
// Step   I.2 of 3: deal with the calibration problem of stage I 
// Step   I.3 of 3: deal with the propagation problem of stage I
// Step  II.1 of 3: code very user specific to stage II
// Step  II.2 of 3: deal with the calibration problem of stage II
// Step  II.3 of 3: deal with the propagation problem of stage II
// Step III.1 of 1: compare the cdf's of stages I and II
//********************************************************
template<class P_V,class P_M,class Q_V,class Q_M>
void 
uqAppl(const uqBaseEnvironmentClass& env)
{
  if (env.rank() == 0) {
    std::cout << "Beginning run of 'uqTgaEx5' example\n"
              << std::endl;
  }

  UQ_FATAL_TEST_MACRO(env.isThereInputFile() == false,
                      env.rank(),
                      "uqAppl()",
                      "input file must be specified in command line, after the '-i' option");

  //******************************************************
  // Read Ascii file with important information on both calibration problems.
  //******************************************************
  uqAsciiTableClass<P_V,P_M> calTable(env,
                                      2,    // # of rows
                                      3,    // # of cols after 'parameter name': min + max + initial value for Markov chain
                                      NULL, // All extra columns are of 'double' type
                                      "cal.tab");

  const EpetraExt::DistArray<std::string>& paramNames = calTable.stringColumn(0);
  P_V                                      s1_minValues    (calTable.doubleColumn(1));
  P_V                                      s1_maxValues    (calTable.doubleColumn(2));
  P_V                                      s1_initialValues(calTable.doubleColumn(3));

  //******************************************************
  // Read Ascii file with important information on both propagation problems.
  //******************************************************
  uqAsciiTableClass<P_V,P_M> proTable(env,
                                      1,    // # of rows
                                      0,    // # of cols after 'parameter name': none
                                      NULL, // All extra columns are of 'double' type
                                      "pro.tab");

  const EpetraExt::DistArray<std::string>& qoiNames = proTable.stringColumn(0);

  double beta3         = 50.;
  double criticalMass3 = .25;

  //*****************************************************
  // Step I.1 of 3: Code very specific to this TGA example
  //*****************************************************

  int iRC;
  struct timeval timevalRef;
  iRC = gettimeofday(&timevalRef, NULL);
  if (env.rank() == 0) {
    std::cout << "Beginning stage 1 at " << ctime(&timevalRef.tv_sec)
              << std::endl;
  }

  // Open input file on experimental data
  FILE *inp;
  inp = fopen("s1_global5.dat","r");

  // Read kinetic parameters and convert heating rate to K/s
  double tmpA, tmpE, beta1, variance1, criticalMass1;
  fscanf(inp,"%lf %lf %lf %lf %lf",&tmpA,&tmpE,&beta1,&variance1,&criticalMass1);
  beta1 /= 60.;
  
  // Read experimental data
  std::vector<double> Te1(11,0.);
  std::vector<double> Me1(11,0.);

  unsigned int numObservations = 0;
  double tmpTe;
  double tmpMe;
  while (fscanf(inp,"%lf %lf",&tmpTe,&tmpMe) != EOF) {
    UQ_FATAL_TEST_MACRO((numObservations >= Te1.size()),
                        env.rank(),
                        "uqAppl(), in uqTgaEx.h",
                        "input file has too many observations");
    Te1[numObservations] = tmpTe;
    Me1[numObservations] = tmpMe;
    numObservations++;
  }
  UQ_FATAL_TEST_MACRO((numObservations != Te1.size()),
                      env.rank(),
                      "uqAppl(), in uqTgaEx.h",
                      "input file has a smaller number of observations than expected");

  // Close input file on experimental data
  fclose(inp);

  //******************************************************
  // Usually, spaces are the same throughout different problems.
  // If this is the case, we can instantiate them here, just once.
  //******************************************************
  uqVectorSpaceClass<P_V,P_M> paramSpace(env,
                                         "param_", // Extra prefix before the default "space_" prefix
                                         calTable.numRows(),
                                         &paramNames);
  uqVectorSpaceClass<Q_V,Q_M> qoiSpace  (env,
                                         "qoi_",   // Extra prefix before the default "space_" prefix
                                         proTable.numRows(),
                                         &qoiNames);

  //******************************************************
  // Step I.2 of 3: deal with the calibration problem
  //******************************************************

  // Stage I (s1): Prior vector rv
  uqUniformVectorRVClass<P_V,P_M> s1_calibPriorRv("s1_cal_prior_", // Extra prefix before the default "rv_" prefix
                                                   paramSpace,
                                                   s1_minValues,
                                                   s1_maxValues);

  // Stage I (s1): 1Likelihood function object: -2*ln[likelihood]
  calibLikelihoodRoutine_DataType<P_V,P_M> s1_calibLikelihoodRoutine_Data;
  s1_calibLikelihoodRoutine_Data.beta     = beta1;
  s1_calibLikelihoodRoutine_Data.variance = variance1;
  s1_calibLikelihoodRoutine_Data.Te       = &Te1; // temperatures
  s1_calibLikelihoodRoutine_Data.Me       = &Me1; // relative masses
  uqGenericVectorPdfClass<P_V,P_M> s1_calibLikelihoodFunctionObj("s1_cal_prior_like_", // Extra prefix before the default "genpd_" prefix
                                                                 paramSpace,
                                                                 calibLikelihoodRoutine<P_V,P_M>,
                                                                 (void *) &s1_calibLikelihoodRoutine_Data,
                                                                 true); // the routine computes [-2.*ln(Likelihood)]

  // Stage I (s1): Posterior vector rv
  uqGenericVectorRVClass<P_V,P_M> s1_calibPostRv("s1_cal_post_", // Extra prefix before the default "rv_" prefix
                                                 paramSpace);

  // Stage I (s1): Calibration problem
  uqCalibProblemClass<P_V,P_M> s1_calibProblem("s1_", // No extra prefix before the default "cal_" prefix
                                               s1_calibPriorRv,
                                               s1_calibLikelihoodFunctionObj,
                                               s1_calibPostRv);

  // Stage I (s1): Solve the calibration problem: set 'pdf' and 'realizer' of 's1_calibPostRv'
  P_M* s1_calibProposalCovMatrix = s1_calibPostRv.imageSpace().newGaussianMatrix(s1_calibPriorRv.pdf().domainVarianceValues(),
                                                                                 s1_initialValues);
  s1_calibProblem.solveWithBayesMarkovChain(s1_initialValues,
                                           *s1_calibProposalCovMatrix,
                                            NULL); // use default kernel from library

  //******************************************************
  // Step I.3 of 3: deal with the propagation problem
  //******************************************************

  // Stage I (s1): Input param vector rv for propagation = output posterior vector rv of calibration

  // Stage I (s1): Qoi function object
  propagQoiRoutine_DataType<P_V,P_M,Q_V,Q_M> s1_propagQoiRoutine_Data;
  s1_propagQoiRoutine_Data.beta         = beta3;
  s1_propagQoiRoutine_Data.criticalMass = criticalMass3;
  uqGenericVectorFunctionClass<P_V,P_M,Q_V,Q_M> s1_propagQoiFunctionObj("s1_pro_qoi_", // Extra prefix before the default "func_" prefix
                                                                        paramSpace,
                                                                        qoiSpace,
                                                                        propagQoiRoutine<P_V,P_M,Q_V,Q_M>,
                                                                        (void *) &s1_propagQoiRoutine_Data);

  // Stage I (s1): Qoi vector rv
  uqGenericVectorRVClass<Q_V,Q_M> s1_propagQoiRv("s1_pro_qoi_", // Extra prefix before the default "rv_" prefix
                                                 qoiSpace);

  // Stage I (s1): Propagation problem
  uqPropagProblemClass<P_V,P_M,Q_V,Q_M> s1_propagProblem("s1_",          // No extra prefix before the default "pro_" prefix
                                                         s1_calibPostRv, // propagation input = calibration output
                                                         s1_propagQoiFunctionObj,
                                                         s1_propagQoiRv);

  // Stage I (s1): Solve the propagation problem: set 'realizer' and 'cdf' of 's1_propagQoiRv'
  s1_propagProblem.solveWithMonteCarlo(); // no extra user entities needed for Monte Carlo algorithm

  struct timeval timevalNow;
  iRC = gettimeofday(&timevalNow, NULL);
  if (env.rank() == 0) {
    std::cout << "Ending stage 1 at " << ctime(&timevalNow.tv_sec)
              << "Total s1 run time = " << timevalNow.tv_sec - timevalRef.tv_sec
              << " seconds"
              << std::endl;
  }

  //*****************************************************
  // Step II.1 of 3: Code very specific to this TGA example
  //*****************************************************

  iRC = gettimeofday(&timevalRef, NULL);
  if (env.rank() == 0) {
    std::cout << "Beginning stage 2 at " << ctime(&timevalRef.tv_sec)
              << std::endl;
  }

  // Open input file on experimental data
  inp = fopen("s2_global5.dat","r");

  // Read kinetic parameters and convert heating rate to K/s
  double beta2, variance2, criticalMass2;
  fscanf(inp,"%lf %lf %lf %lf %lf",&tmpA,&tmpE,&beta2,&variance2,&criticalMass2);
  beta2 /= 60.;
  
  // Read experimental data
  std::vector<double> Te2(11,0.);
  std::vector<double> Me2(11,0.);

  numObservations = 0;
  while (fscanf(inp,"%lf %lf",&tmpTe,&tmpMe) != EOF) {
    UQ_FATAL_TEST_MACRO((numObservations >= Te2.size()),
                        env.rank(),
                        "uqAppl(), in uqTgaEx.h",
                        "input file has too many observations");
    Te2[numObservations] = tmpTe;
    Me2[numObservations] = tmpMe;
    numObservations++;
  }
  UQ_FATAL_TEST_MACRO((numObservations != Te2.size()),
                      env.rank(),
                      "uqAppl(), in uqTgaEx.h",
                      "input file has a smaller number of observations than expected");

  // Close input file on experimental data
  fclose(inp);

  //******************************************************
  // Step II.2 of 3: deal with the calibration problem
  //******************************************************

  // Stage II (s2): Prior vector rv = posterior vector rv of stage I (s1)

  // Stage II (s2): 1Likelihood function object: -2*ln[likelihood]
  calibLikelihoodRoutine_DataType<P_V,P_M> s2_calibLikelihoodRoutine_Data;
  s2_calibLikelihoodRoutine_Data.beta     = beta2;
  s2_calibLikelihoodRoutine_Data.variance = variance2;
  s2_calibLikelihoodRoutine_Data.Te       = &Te2; // temperatures
  s2_calibLikelihoodRoutine_Data.Me       = &Me2; // relative masses
  uqGenericVectorPdfClass<P_V,P_M> s2_calibLikelihoodFunctionObj("s2_cal_prior_like_", // Extra prefix before the default "genpd_" prefix
                                                                 paramSpace,
                                                                 calibLikelihoodRoutine<P_V,P_M>,
                                                                 (void *) &s2_calibLikelihoodRoutine_Data,
                                                                 true); // the routine computes [-2.*ln(Likelihood)]

  // Stage II (s2): Posterior vector rv
  uqGenericVectorRVClass<P_V,P_M> s2_calibPostRv("s2_cal_post_", // Extra prefix before the default "rv_" prefix
                                                 paramSpace);

  // Stage II (s2): Calibration problem
  uqCalibProblemClass<P_V,P_M> s2_calibProblem("s2_", // No extra prefix before the default "cal_" prefix
                                               s1_calibPostRv, // s2 calibration input = s1 calibration output
                                               s2_calibLikelihoodFunctionObj,
                                               s2_calibPostRv);

  // Stage II (s2): Solve the calibration problem: set 'pdf' and 'realizer' of 's2_calibPostRv'
  P_M* s2_calibProposalCovMatrix = s1_calibPostRv.imageSpace().newGaussianMatrix(s1_calibPostRv.realizer().imageVarianceValues(),  // Use 'realizer()' because the posterior rv was computed with Markov Chain
                                                                                 s1_calibPostRv.realizer().imageExpectedValues()); // Use these values as the initial values
  s2_calibProblem.solveWithBayesMarkovChain(s1_calibPostRv.realizer().imageExpectedValues(),
                                           *s2_calibProposalCovMatrix,
                                            NULL); // use default kernel from library

  //******************************************************
  // Step II.3 of 3: deal with the propagation problem
  //******************************************************

  // Stage II (s2): Input param vector rv for propagation = output posterior vector rv of calibration

  // Stage II (s2): Qoi function object
  propagQoiRoutine_DataType<P_V,P_M,Q_V,Q_M> s2_propagQoiRoutine_Data;
  s2_propagQoiRoutine_Data.beta          = beta3;
  s2_propagQoiRoutine_Data.criticalMass  = criticalMass3;
  uqGenericVectorFunctionClass<P_V,P_M,Q_V,Q_M> s2_propagQoiFunctionObj("s2_pro_qoi_", // Extra prefix before the default "func_" prefix
                                                                        paramSpace,
                                                                        qoiSpace,
                                                                        propagQoiRoutine<P_V,P_M,Q_V,Q_M>,
                                                                        (void *) &s2_propagQoiRoutine_Data);

  // Stage II (s2): Qoi vector rv: set 'realizer' and 'cdf' of 's2_propagQoiRv'
  uqGenericVectorRVClass<Q_V,Q_M> s2_propagQoiRv("s2_pro_qoi_", // Extra prefix before the default "rv_" prefix
                                                 qoiSpace);

  // Stage II (s2): Propagation problem
  uqPropagProblemClass<P_V,P_M,Q_V,Q_M> s2_propagProblem("s2_",          // No extra prefix before the default "pro_" prefix
                                                         s2_calibPostRv, // propagation input = calibration output
                                                         s2_propagQoiFunctionObj,
                                                         s2_propagQoiRv);

  // Stage II (s2): Solve the propagation problem
  s2_propagProblem.solveWithMonteCarlo(); // no extra user entities needed for Monte Carlo algorithm

  iRC = gettimeofday(&timevalNow, NULL);
  if (env.rank() == 0) {
    std::cout << "Ending stage 2 at " << ctime(&timevalNow.tv_sec)
              << "Total s2 run time = " << timevalNow.tv_sec - timevalRef.tv_sec
              << " seconds"
              << std::endl;
  }

  //******************************************************
  // Step III.1 of 1: compare the cdf's of stages I and II
  //******************************************************

  iRC = gettimeofday(&timevalRef, NULL);
  if (env.rank() == 0) {
    std::cout << "Beginning stage 3 at " << ctime(&timevalRef.tv_sec)
              << std::endl;
  }

  if (s1_propagProblem.computeSolutionFlag() &&
      s2_propagProblem.computeSolutionFlag()) {
    Q_V* epsilonVec = s1_propagQoiRv.imageSpace().newVector(0.02);
    Q_V cdfDistancesVec(s1_propagQoiRv.imageSpace().zeroVector());
    horizontalDistances(s1_propagQoiRv.cdf(),
                        s2_propagQoiRv.cdf(),
                        *epsilonVec,
                        cdfDistancesVec);
    if (env.rank() == 0) {
      std::cout << "For epsilonVec = "    << *epsilonVec
                << ", cdfDistancesVec = " << cdfDistancesVec
                << std::endl;
    }

    // Test independence of 'distance' w.r.t. order of cdfs
    horizontalDistances(s2_propagQoiRv.cdf(),
                        s1_propagQoiRv.cdf(),
                        *epsilonVec,
                        cdfDistancesVec);
    if (env.rank() == 0) {
      std::cout << "For epsilonVec = "    << *epsilonVec
                << ", cdfDistancesVec (swithced order of cdfs) = " << cdfDistancesVec
                << std::endl;
    }

    // Epsilon = 0.04
    epsilonVec->cwSet(0.04);
    horizontalDistances(s1_propagQoiRv.cdf(),
                        s2_propagQoiRv.cdf(),
                        *epsilonVec,
                        cdfDistancesVec);
    if (env.rank() == 0) {
      std::cout << "For epsilonVec = "    << *epsilonVec
                << ", cdfDistancesVec = " << cdfDistancesVec
                << std::endl;
    }

    // Epsilon = 0.06
    epsilonVec->cwSet(0.06);
    horizontalDistances(s1_propagQoiRv.cdf(),
                        s2_propagQoiRv.cdf(),
                        *epsilonVec,
                        cdfDistancesVec);
    if (env.rank() == 0) {
      std::cout << "For epsilonVec = "    << *epsilonVec
                << ", cdfDistancesVec = " << cdfDistancesVec
                << std::endl;
    }

    // Epsilon = 0.08
    epsilonVec->cwSet(0.08);
    horizontalDistances(s1_propagQoiRv.cdf(),
                        s2_propagQoiRv.cdf(),
                        *epsilonVec,
                        cdfDistancesVec);
    if (env.rank() == 0) {
      std::cout << "For epsilonVec = "    << *epsilonVec
                << ", cdfDistancesVec = " << cdfDistancesVec
                << std::endl;
    }

    // Epsilon = 0.10
    epsilonVec->cwSet(0.10);
    horizontalDistances(s1_propagQoiRv.cdf(),
                        s2_propagQoiRv.cdf(),
                        *epsilonVec,
                        cdfDistancesVec);
    if (env.rank() == 0) {
      std::cout << "For epsilonVec = "    << *epsilonVec
                << ", cdfDistancesVec = " << cdfDistancesVec
                << std::endl;
    }

    delete epsilonVec;
  }

  iRC = gettimeofday(&timevalNow, NULL);
  if (env.rank() == 0) {
    std::cout << "Ending stage 3 at " << ctime(&timevalNow.tv_sec)
              << "Total s3 run time = " << timevalNow.tv_sec - timevalRef.tv_sec
              << " seconds"
              << std::endl;
  }

  //******************************************************
  // Release memory before leaving routine.
  //******************************************************
  delete s2_calibProposalCovMatrix;
  delete s1_calibProposalCovMatrix;

  if (env.rank() == 0) {
    std::cout << "Finishing run of 'uqTgaEx5' example"
              << std::endl;
  }

  return;
}
#endif // __UQ_TGA_EX5_H__
