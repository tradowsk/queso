/* uq/libs/queso/inc/uqMarkovChainSG1.h
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

#ifndef __UQ_MAC_SG1_H__
#define __UQ_MAC_SG1_H__

#undef UQ_USES_TK_CLASS
#undef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
#define UQ_MAC_SG_REQUIRES_TARGET_DISTRIBUTION_ONLY

#define UQ_MAC_SG_MARKOV_CHAIN_TYPE           1
#define UQ_MAC_SG_WHITE_NOISE_CHAIN_TYPE      2
#define UQ_MAC_SG_UNIFORM_CHAIN_TYPE          3
#define UQ_MAC_SG_FILENAME_FOR_NO_OUTPUT_FILE "."

// _ODV = option default value
#define UQ_MAC_SG_CHAIN_TYPE_ODV                       UQ_MAC_SG_MARKOV_CHAIN_TYPE
#define UQ_MAC_SG_CHAIN_SIZE_ODV                       100
#define UQ_MAC_SG_CHAIN_OUTPUT_FILE_NAME_ODV           UQ_MAC_SG_FILENAME_FOR_NO_OUTPUT_FILE
#define UQ_MAC_SG_CHAIN_USE2_ODV                       0
#define UQ_MAC_SG_CHAIN_GENERATE_EXTRA_ODV             0
#define UQ_MAC_SG_CHAIN_DISPLAY_PERIOD_ODV             500
#define UQ_MAC_SG_CHAIN_MEASURE_RUN_TIMES_ODV          0
#define UQ_MAC_SG_CHAIN_WRITE_ODV                      0
#define UQ_MAC_SG_CHAIN_COMPUTE_STATS_ODV              0
#define UQ_MAC_SG_UNIQUE_CHAIN_GENERATE_ODV            0
#define UQ_MAC_SG_UNIQUE_CHAIN_WRITE_ODV               0
#define UQ_MAC_SG_UNIQUE_CHAIN_COMPUTE_STATS_ODV       0
#define UQ_MAC_SG_FILTERED_CHAIN_GENERATE_ODV          0
#define UQ_MAC_SG_FILTERED_CHAIN_DISCARDED_PORTION_ODV 0.
#define UQ_MAC_SG_FILTERED_CHAIN_LAG_ODV               1
#define UQ_MAC_SG_FILTERED_CHAIN_WRITE_ODV             0
#define UQ_MAC_SG_FILTERED_CHAIN_COMPUTE_STATS_ODV     0
#define UQ_MAC_SG_TK_USE_LOCAL_HESSIAN_ODV             0
#define UQ_MAC_SG_TK_USE_NEWTON_COMPONENT_ODV          0
#define UQ_MAC_SG_DR_MAX_NUM_EXTRA_STAGES_ODV          0
#define UQ_MAC_SG_DR_SCALES_FOR_EXTRA_STAGES_ODV       "1."
#define UQ_MAC_SG_AM_INIT_NON_ADAPT_INT_ODV            0
#define UQ_MAC_SG_AM_ADAPT_INTERVAL_ODV                0
#define UQ_MAC_SG_AM_ETA_ODV                           1.
#define UQ_MAC_SG_AM_EPSILON_ODV                       1.e-5

#include <uqTKGroup.h>
#include <uqChainStatisticalOptions.h>
#include <uqVectorRV.h>
#include <uqVectorSpace.h>
#include <uqMarkovChainPosition.h>
#include <uqMiscellaneous.h>
#include <uqSequenceOfVectors.h>
#include <uqArrayOfSequences.h>
#include <sys/time.h>
#include <fstream>

/*! A templated class that implements a Bayesian Markov Chain Distribution Calculator
 */
template <class P_V,class P_M>
class uqMarkovChainSGClass
{
public:
  uqMarkovChainSGClass(const char*                         prefix,                  /*! Prefix.                     */
                       const uqBaseVectorRVClass<P_V,P_M>& sourceRv,                /*! The source random variable. */
                       const P_V&                          initialPosition,         /*! First position of chain.    */
                       const P_M*                          inputProposalCovMatrix); /*! Proposal covariance matrix. */ 
 ~uqMarkovChainSGClass();

  void   generateSequence         (uqBaseVectorSequenceClass<P_V,P_M>& workingChain); /*! */

  void   print                    (std::ostream& os) const;


private:
  void   resetChainAndRelatedInfo ();
  void   defineMyOptions          (po::options_description&                             optionsDesc);
  void   getMyOptionValues        (po::options_description&                             optionsDesc);

  void   generateWhiteNoiseChain  (unsigned int                                         chainSize,
                                   uqBaseVectorSequenceClass<P_V,P_M>&                  workingChain);
  void   generateUniformChain     (unsigned int                                         chainSize,
                                   uqBaseVectorSequenceClass<P_V,P_M>&                  workingChain);
  void   generateFullChain        (const P_V&                                           valuesOf1stPosition,
                                   uqBaseVectorSequenceClass<P_V,P_M>&                  workingChain,
                                   unsigned int                                         chainSize);
  void   updateAdaptedCovMatrix   (const uqBaseVectorSequenceClass<P_V,P_M>&            subChain,
                                   unsigned int                                         idOfFirstPositionInSubChain,
                                   double&                                              lastChainSize,
                                   P_V&                                                 lastMean,
                                   P_M&                                                 lastAdaptedCovMatrix);

#ifdef UQ_USES_TK_CLASS
#else
  int    computeInitialCholFactors();
  void   updateTK                 ();
  double logProposal              (const uqMarkovChainPositionClass<P_V>&               x,
                                   const uqMarkovChainPositionClass<P_V>&               y,
                                   unsigned int                                         idOfProposalCovMatrix);
  double logProposal              (const std::vector<uqMarkovChainPositionClass<P_V>*>& inputPositions);
#endif
  double alpha                    (const uqMarkovChainPositionClass<P_V>&               x,
                                   const uqMarkovChainPositionClass<P_V>&               y,
                                   double*                                              alphaQuotientPtr = NULL);
  double alpha                    (const std::vector<uqMarkovChainPositionClass<P_V>*>& inputPositions);
  bool   acceptAlpha              (double                                               alpha);

  int    writeInfo                (const uqBaseVectorSequenceClass<P_V,P_M>&            workingChain,
                                   std::ofstream&                                       ofs) const;
                                 //const P_M*                                           mahalanobisMatrix = NULL,
                                 //bool                                                 applyMahalanobisInvert = true) const;

  const uqBaseEnvironmentClass&                    m_env;
        std::string                                m_prefix;
  const uqVectorSpaceClass   <P_V,P_M>&            m_vectorSpace;
  const uqBaseVectorPdfClass <P_V,P_M>&            m_targetPdf;
        P_V                                        m_initialPosition;
  const P_M*                                       m_initialProposalCovMatrix;
        bool                                       m_nullInputProposalCovMatrix;

        po::options_description*                   m_optionsDesc;
        std::string                                m_option_help;
        std::string                                m_option_chain_type;
        std::string                                m_option_chain_size;
        std::string                                m_option_chain_outputFileName;
        std::string                                m_option_chain_use2;
        std::string                                m_option_chain_generateExtra;
        std::string                                m_option_chain_displayPeriod;
        std::string                                m_option_chain_measureRunTimes;
        std::string                                m_option_chain_write;
        std::string                                m_option_chain_computeStats;
        std::string                                m_option_uniqueChain_generate;
        std::string                                m_option_uniqueChain_write;
        std::string                                m_option_uniqueChain_computeStats;
        std::string                                m_option_filteredChain_generate;
        std::string                                m_option_filteredChain_discardedPortion;
        std::string                                m_option_filteredChain_lag;
        std::string                                m_option_filteredChain_write;
        std::string                                m_option_filteredChain_computeStats;
	std::string                                m_option_tk_useLocalHessian;
	std::string                                m_option_tk_useNewtonComponent;
        std::string                                m_option_dr_maxNumExtraStages;
        std::string                                m_option_dr_scalesForExtraStages;
        std::string                                m_option_am_initialNonAdaptInterval;
        std::string                                m_option_am_adaptInterval;
        std::string                                m_option_am_eta;
        std::string                                m_option_am_epsilon;

        unsigned int                               m_chainType;
        unsigned int                               m_chainSize;
        std::string                                m_chainOutputFileName;
        bool                                       m_chainGenerateExtra;
        unsigned int                               m_chainDisplayPeriod;
        bool                                       m_chainMeasureRunTimes;
        bool                                       m_chainWrite;
        bool                                       m_chainComputeStats;
        uqChainStatisticalOptionsClass*            m_chainStatisticalOptions;

        bool                                       m_uniqueChainGenerate;
        bool                                       m_uniqueChainWrite;
        bool                                       m_uniqueChainComputeStats;
        uqChainStatisticalOptionsClass*            m_uniqueChainStatisticalOptions;

        bool                                       m_filteredChainGenerate;
        double                                     m_filteredChainDiscardedPortion; // input or set during run time
        unsigned int                               m_filteredChainLag;              // input or set during run time
        bool                                       m_filteredChainWrite;
        bool                                       m_filteredChainComputeStats;
        uqChainStatisticalOptionsClass*            m_filteredChainStatisticalOptions;

        bool                                       m_tkUseLocalHessian;
        bool                                       m_tkUseNewtonComponent;
        unsigned int                               m_drMaxNumExtraStages;
        std::vector<double>                        m_drScalesForCovMatrices;
        unsigned int                               m_amInitialNonAdaptInterval;
        unsigned int                               m_amAdaptInterval;
        double                                     m_amEta;
        double                                     m_amEpsilon;

        uqScaledCovMatrixTKGroupClass   <P_V,P_M>* m_tk1;
        uqHessianCovMatricesTKGroupClass<P_V,P_M>* m_tk2;
        bool                                       m_tkIsSymmetric;
#ifdef UQ_USES_TK_CLASS
#else
        std::vector<P_M*>                          m_lowerCholProposalCovMatrices;
        std::vector<P_M*>                          m_proposalCovMatrices;
#ifdef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
        std::vector<P_M*>                          m_upperCholProposalPrecMatrices;
        std::vector<P_M*>                          m_proposalPrecMatrices;
#endif
#endif
        std::vector<unsigned int>                  m_idsOfUniquePositions;
        std::vector<double>                        m_logTargets;
        std::vector<double>                        m_alphaQuotients;
        double                                     m_chainRunTime;
        unsigned int                               m_numRejections;
        unsigned int                               m_numOutOfBounds;
        double                                     m_lastChainSize;
        P_V*                                       m_lastMean;
        P_M*                                       m_lastAdaptedCovMatrix;
};

template<class P_V,class P_M>
std::ostream& operator<<(std::ostream& os, const uqMarkovChainSGClass<P_V,P_M>& obj);

#include <uqMarkovChainSG2.h>

template<class P_V,class P_M>
uqMarkovChainSGClass<P_V,P_M>::uqMarkovChainSGClass(
  const char*                         prefix,
  const uqBaseVectorRVClass<P_V,P_M>& sourceRv,
  const P_V&                          initialPosition,
  const P_M*                          inputProposalCovMatrix)
  :
  m_env                                  (sourceRv.env()),
  m_prefix                               ((std::string)(prefix) + "mc_"),
  m_vectorSpace                          (sourceRv.imageSpace()),
  m_targetPdf                            (sourceRv.pdf()),
  m_initialPosition                      (initialPosition),
  m_initialProposalCovMatrix             (inputProposalCovMatrix),
  m_nullInputProposalCovMatrix           (inputProposalCovMatrix == NULL),
  m_optionsDesc                          (new po::options_description("Bayesian Markov chain options")),
  m_option_help                          (m_prefix + "help"                          ),
  m_option_chain_type                    (m_prefix + "chain_type"                    ),
  m_option_chain_size                    (m_prefix + "chain_size"                    ),
  m_option_chain_outputFileName          (m_prefix + "chain_outputFileName"          ),
  m_option_chain_use2                    (m_prefix + "chain_use2"                    ),
  m_option_chain_generateExtra           (m_prefix + "chain_generateExtra"           ),
  m_option_chain_displayPeriod           (m_prefix + "chain_displayPeriod"           ),
  m_option_chain_measureRunTimes         (m_prefix + "chain_measureRunTimes"         ),
  m_option_chain_write                   (m_prefix + "chain_write"                   ),
  m_option_chain_computeStats            (m_prefix + "chain_computeStats"            ),
  m_option_uniqueChain_generate          (m_prefix + "uniqueChain_generate"          ),
  m_option_uniqueChain_write             (m_prefix + "uniqueChain_write"             ),
  m_option_uniqueChain_computeStats      (m_prefix + "uniqueChain_computeStats"      ),
  m_option_filteredChain_generate        (m_prefix + "filteredChain_generate"        ),
  m_option_filteredChain_discardedPortion(m_prefix + "filteredChain_discardedPortion"),
  m_option_filteredChain_lag             (m_prefix + "filteredChain_lag"             ),
  m_option_filteredChain_write           (m_prefix + "filteredChain_write"           ),
  m_option_filteredChain_computeStats    (m_prefix + "filteredChain_computeStats"    ),
  m_option_tk_useLocalHessian      (m_prefix + "proposal_useLocalHessian"      ),
  m_option_tk_useNewtonComponent   (m_prefix + "proposal_useNewtonComponent"   ),
  m_option_dr_maxNumExtraStages          (m_prefix + "dr_maxNumExtraStages"          ),
  m_option_dr_scalesForExtraStages       (m_prefix + "dr_scalesForExtraStages"       ),
  m_option_am_initialNonAdaptInterval    (m_prefix + "am_initialNonAdaptInterval"    ),
  m_option_am_adaptInterval              (m_prefix + "am_adaptInterval"              ),
  m_option_am_eta                        (m_prefix + "am_eta"                        ),
  m_option_am_epsilon                    (m_prefix + "am_epsilon"                    ),
  m_chainType                            (UQ_MAC_SG_CHAIN_TYPE_ODV),
  m_chainSize                            (UQ_MAC_SG_CHAIN_SIZE_ODV),
  m_chainOutputFileName                  (UQ_MAC_SG_CHAIN_OUTPUT_FILE_NAME_ODV),
  m_chainGenerateExtra                   (UQ_MAC_SG_CHAIN_GENERATE_EXTRA_ODV),
  m_chainDisplayPeriod                   (UQ_MAC_SG_CHAIN_DISPLAY_PERIOD_ODV),
  m_chainMeasureRunTimes                 (UQ_MAC_SG_CHAIN_MEASURE_RUN_TIMES_ODV),
  m_chainWrite                           (UQ_MAC_SG_CHAIN_WRITE_ODV),
  m_chainComputeStats                    (UQ_MAC_SG_CHAIN_COMPUTE_STATS_ODV),
  m_chainStatisticalOptions              (NULL),
  m_uniqueChainGenerate                  (UQ_MAC_SG_UNIQUE_CHAIN_GENERATE_ODV),
  m_uniqueChainWrite                     (UQ_MAC_SG_UNIQUE_CHAIN_WRITE_ODV),
  m_uniqueChainComputeStats              (UQ_MAC_SG_UNIQUE_CHAIN_COMPUTE_STATS_ODV),
  m_uniqueChainStatisticalOptions        (NULL),
  m_filteredChainGenerate                (UQ_MAC_SG_FILTERED_CHAIN_GENERATE_ODV),
  m_filteredChainDiscardedPortion        (UQ_MAC_SG_FILTERED_CHAIN_DISCARDED_PORTION_ODV),
  m_filteredChainLag                     (UQ_MAC_SG_FILTERED_CHAIN_LAG_ODV),
  m_filteredChainWrite                   (UQ_MAC_SG_FILTERED_CHAIN_WRITE_ODV),
  m_filteredChainComputeStats            (UQ_MAC_SG_FILTERED_CHAIN_COMPUTE_STATS_ODV),
  m_filteredChainStatisticalOptions      (NULL),
  m_tkUseLocalHessian                    (UQ_MAC_SG_TK_USE_LOCAL_HESSIAN_ODV),
  m_tkUseNewtonComponent                 (UQ_MAC_SG_TK_USE_NEWTON_COMPONENT_ODV),
  m_drMaxNumExtraStages                  (UQ_MAC_SG_DR_MAX_NUM_EXTRA_STAGES_ODV),
  m_drScalesForCovMatrices               (0),//0.),
  m_amInitialNonAdaptInterval            (UQ_MAC_SG_AM_INIT_NON_ADAPT_INT_ODV),
  m_amAdaptInterval                      (UQ_MAC_SG_AM_ADAPT_INTERVAL_ODV),
  m_amEta                                (UQ_MAC_SG_AM_ETA_ODV),
  m_amEpsilon                            (UQ_MAC_SG_AM_EPSILON_ODV),
  m_tk1                                  (NULL),
  m_tk2                                  (NULL),
  m_tkIsSymmetric                        (true),
#ifdef UQ_USES_TK_CLASS
#else
  m_lowerCholProposalCovMatrices         (1),//NULL),
  m_proposalCovMatrices                  (1),//NULL),
#ifdef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
  m_upperCholProposalPrecMatrices        (1),//NULL),
  m_proposalPrecMatrices                 (1),//NULL),
#endif
#endif
  m_idsOfUniquePositions                 (0),//0.),
  m_logTargets                           (0),//0.),
  m_alphaQuotients                       (0),//0.),
  m_chainRunTime                         (0.),
  m_numRejections                        (0),
  m_numOutOfBounds                       (0),
  m_lastChainSize                        (0),
  m_lastMean                             (NULL),
  m_lastAdaptedCovMatrix                 (NULL)
{
  if (m_env.rank() == 0) std::cout << "Entering uqMarkovChainSGClass<P_V,P_M>::constructor()"
                                   << std::endl;

  defineMyOptions                (*m_optionsDesc);
  m_env.scanInputFileForMyOptions(*m_optionsDesc);
  getMyOptionValues              (*m_optionsDesc);

  if (m_env.rank() == 0) std::cout << "In uqMarkovChainSGClass<P_V,P_M>::constructor()"
                                   << ": after getting values of options with prefix '" << m_prefix
                                   << "', state of  object is:"
                                   << "\n" << *this
                                   << std::endl;

  /////////////////////////////////////////////////////////////////
  // Instantiate the appropriate TK
  /////////////////////////////////////////////////////////////////
  if (m_tkUseLocalHessian) {
    // Set current proposal cov matrix = local Hessian at initial position

    if (m_tkUseNewtonComponent) {
      // Set current proposal exp vector = initial position - H^{-1}*\grad[-2*ln(target density)]
    }

    m_tk2 = new uqHessianCovMatricesTKGroupClass<P_V,P_M>(m_prefix.c_str(),
                                                          m_vectorSpace,
                                                          m_targetPdf,
                                                          m_drScalesForCovMatrices.size());
  }
  else {
    if (m_initialProposalCovMatrix == NULL) {
      UQ_FATAL_TEST_MACRO(true,
                          m_env.rank(),
                          "uqMarkovChainSGClass<P_V,P_M>::constructor()",
                          "proposal cov matrix should have been passed by user, since, according to the input algorithm options, local Hessians will not be used in the proposal");
    }

    m_tk1 = new uqScaledCovMatrixTKGroupClass<P_V,P_M>(m_prefix.c_str(),
                                                       m_vectorSpace,
                                                      *m_initialProposalCovMatrix,
                                                       m_drScalesForCovMatrices);
  }


  if (m_chainComputeStats        ) m_chainStatisticalOptions         = new uqChainStatisticalOptionsClass(m_env,m_prefix + "chain_"        );
  if (m_uniqueChainComputeStats  ) m_uniqueChainStatisticalOptions   = new uqChainStatisticalOptionsClass(m_env,m_prefix + "uniqueChain_"  );
  if (m_filteredChainComputeStats) m_filteredChainStatisticalOptions = new uqChainStatisticalOptionsClass(m_env,m_prefix + "filteredChain_");

  if (m_env.rank() == 0) std::cout << "Leaving uqMarkovChainSGClass<P_V,P_M>::constructor()"
                                   << std::endl;
}

template<class P_V,class P_M>
uqMarkovChainSGClass<P_V,P_M>::~uqMarkovChainSGClass()
{
  //std::cout << "Entering uqMarkovChainSGClass<P_V,P_M>::destructor()"
  //          << std::endl;

  resetChainAndRelatedInfo();
  //if (m_lowerCholProposalCovMatrices[0]) delete m_lowerCholProposalCovMatrices[0]; // Loop inside 'resetChainAndRelatedInfo()' deletes just from position '1' on

  if (m_filteredChainStatisticalOptions) delete m_filteredChainStatisticalOptions;
  if (m_uniqueChainStatisticalOptions  ) delete m_uniqueChainStatisticalOptions;
  if (m_chainStatisticalOptions        ) delete m_chainStatisticalOptions;
  if (m_optionsDesc                    ) delete m_optionsDesc;

  if (m_tk1                            ) delete m_tk1;
  if (m_tk2                            ) delete m_tk2;
  if (m_nullInputProposalCovMatrix     ) delete m_initialProposalCovMatrix;

  //std::cout << "Leaving uqMarkovChainSGClass<P_V,P_M>::destructor()"
  //          << std::endl;
}

template<class P_V,class P_M>
void
uqMarkovChainSGClass<P_V,P_M>::resetChainAndRelatedInfo()
{
  if (m_lastAdaptedCovMatrix) delete m_lastAdaptedCovMatrix;
  if (m_lastMean)             delete m_lastMean;
  m_lastChainSize  = 0;
  m_numOutOfBounds = 0;
  m_chainRunTime   = 0.;
  m_numRejections  = 0;
  m_alphaQuotients.clear();
  m_logTargets.clear();
  m_idsOfUniquePositions.clear();

#ifdef UQ_USES_TK_CLASS
#else
#ifdef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
  for (unsigned int i = 0; i < m_proposalPrecMatrices.size(); ++i) {
    if (m_proposalPrecMatrices[i]) delete m_proposalPrecMatrices[i];
  }
  for (unsigned int i = 0; i < m_upperCholProposalPrecMatrices.size(); ++i) {
    if (m_upperCholProposalPrecMatrices[i]) delete m_upperCholProposalPrecMatrices[i];
  }
#endif
  for (unsigned int i = 0; i < m_proposalCovMatrices.size(); ++i) {
    if (m_proposalCovMatrices[i]) {
      delete m_proposalCovMatrices[i];
      m_proposalCovMatrices[i] = NULL;
    }
  }
//m_proposalCovMatrices.clear(); // Do not clear
  for (unsigned int i = 0; i < m_lowerCholProposalCovMatrices.size(); ++i) { // Yes, from '1' on
    if (m_lowerCholProposalCovMatrices[i]) {
      delete m_lowerCholProposalCovMatrices[i];
      m_lowerCholProposalCovMatrices[i] = NULL;
    }
  }
//m_lowerCholProposalCovMatrices.clear(); // Do not clear
#endif

  return;
}

template<class P_V,class P_M>
void
uqMarkovChainSGClass<P_V,P_M>::defineMyOptions(
  po::options_description& optionsDesc)
{
  optionsDesc.add_options()
    (m_option_help.c_str(),                                                                                                                     "produce help message for Bayesian Markov chain distr. calculator")
    (m_option_chain_type.c_str(),                     po::value<unsigned int>()->default_value(UQ_MAC_SG_CHAIN_TYPE_ODV                      ), "type of chain (1=Markov, 2=White noise)"                         )
    (m_option_chain_size.c_str(),                     po::value<unsigned int>()->default_value(UQ_MAC_SG_CHAIN_SIZE_ODV                      ), "size of chain"                                                   )
    (m_option_chain_outputFileName.c_str(),           po::value<std::string >()->default_value(UQ_MAC_SG_CHAIN_OUTPUT_FILE_NAME_ODV          ), "name of output file"                                             )
    (m_option_chain_use2.c_str(),                     po::value<bool        >()->default_value(UQ_MAC_SG_CHAIN_USE2_ODV                      ), "use chain2"                                                      )
    (m_option_chain_generateExtra.c_str(),            po::value<bool        >()->default_value(UQ_MAC_SG_CHAIN_GENERATE_EXTRA_ODV            ), "generate extra chains"                                           )
    (m_option_chain_displayPeriod.c_str(),            po::value<unsigned int>()->default_value(UQ_MAC_SG_CHAIN_DISPLAY_PERIOD_ODV            ), "period of message display during chain generation"               )
    (m_option_chain_measureRunTimes.c_str(),          po::value<bool        >()->default_value(UQ_MAC_SG_CHAIN_MEASURE_RUN_TIMES_ODV         ), "measure run times"                                               )
    (m_option_chain_write.c_str(),                    po::value<bool        >()->default_value(UQ_MAC_SG_CHAIN_WRITE_ODV                     ), "write chain values to the output file"                           )
    (m_option_chain_computeStats.c_str(),             po::value<bool        >()->default_value(UQ_MAC_SG_CHAIN_COMPUTE_STATS_ODV             ), "compute statistics on chain"                                     )
  //(m_option_uniqueChain_generate.c_str(),           po::value<bool        >()->default_value(UQ_MAC_SG_UNIQUE_CHAIN_GENERATE_ODV           ), "generate unique chain"                                           )
  //(m_option_uniqueChain_write.c_str(),              po::value<bool        >()->default_value(UQ_MAC_SG_UNIQUE_CHAIN_WRITE_ODV              ), "write unique chain"                                              )
  //(m_option_uniqueChain_computeStats.c_str(),       po::value<bool        >()->default_value(UQ_MAC_SG_UNIQUE_CHAIN_COMPUTE_STATS_ODV      ), "compute statistics on unique chain"                              )
    (m_option_filteredChain_generate.c_str(),         po::value<bool        >()->default_value(UQ_MAC_SG_FILTERED_CHAIN_GENERATE_ODV         ), "generate filtered chain"                                         )
    (m_option_filteredChain_discardedPortion.c_str(), po::value<double      >()->default_value(UQ_MAC_SG_FILTERED_CHAIN_DISCARDED_PORTION_ODV), "initial discarded portion for chain filtering"                   )
    (m_option_filteredChain_lag.c_str(),              po::value<unsigned int>()->default_value(UQ_MAC_SG_FILTERED_CHAIN_LAG_ODV              ), "spacing for chain filtering"                                     )
    (m_option_filteredChain_write.c_str(),            po::value<bool        >()->default_value(UQ_MAC_SG_FILTERED_CHAIN_WRITE_ODV            ), "write filtered chain"                                            )
    (m_option_filteredChain_computeStats.c_str(),     po::value<bool        >()->default_value(UQ_MAC_SG_FILTERED_CHAIN_COMPUTE_STATS_ODV    ), "compute statistics on filtered chain"                            )
    (m_option_tk_useLocalHessian.c_str(),       po::value<bool        >()->default_value(UQ_MAC_SG_TK_USE_LOCAL_HESSIAN_ODV      ), "'proposal' use local Hessian"                                    )
    (m_option_tk_useNewtonComponent.c_str(),    po::value<bool        >()->default_value(UQ_MAC_SG_TK_USE_NEWTON_COMPONENT_ODV   ), "'proposal' use Newton component"                                 )
    (m_option_dr_maxNumExtraStages.c_str(),           po::value<unsigned int>()->default_value(UQ_MAC_SG_DR_MAX_NUM_EXTRA_STAGES_ODV         ), "'dr' maximum number of extra stages"                             )
    (m_option_dr_scalesForExtraStages.c_str(),        po::value<std::string >()->default_value(UQ_MAC_SG_DR_SCALES_FOR_EXTRA_STAGES_ODV      ), "'dr' list of scales for proposal cov matrices from 2nd stage on" )
    (m_option_am_initialNonAdaptInterval.c_str(),     po::value<unsigned int>()->default_value(UQ_MAC_SG_AM_INIT_NON_ADAPT_INT_ODV           ), "'am' initial non adaptation interval"                            )
    (m_option_am_adaptInterval.c_str(),               po::value<unsigned int>()->default_value(UQ_MAC_SG_AM_ADAPT_INTERVAL_ODV               ), "'am' adaptation interval"                                        )
    (m_option_am_eta.c_str(),                         po::value<double      >()->default_value(UQ_MAC_SG_AM_ETA_ODV                          ), "'am' eta"                                                        )
    (m_option_am_epsilon.c_str(),                     po::value<double      >()->default_value(UQ_MAC_SG_AM_EPSILON_ODV                      ), "'am' epsilon"                                                    )
  ;

  return;
}

template<class P_V,class P_M>
void
uqMarkovChainSGClass<P_V,P_M>::getMyOptionValues(
  po::options_description& optionsDesc)
{
  if (m_env.allOptionsMap().count(m_option_help.c_str())) {
    std::cout << optionsDesc
              << std::endl;
  }

  if (m_env.allOptionsMap().count(m_option_chain_type.c_str())) {
    m_chainType = m_env.allOptionsMap()[m_option_chain_type.c_str()].as<unsigned int>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_size.c_str())) {
    m_chainSize = m_env.allOptionsMap()[m_option_chain_size.c_str()].as<unsigned int>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_displayPeriod.c_str())) {
    m_chainDisplayPeriod = m_env.allOptionsMap()[m_option_chain_displayPeriod.c_str()].as<unsigned int>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_measureRunTimes.c_str())) {
    m_chainMeasureRunTimes = m_env.allOptionsMap()[m_option_chain_measureRunTimes.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_write.c_str())) {
    m_chainWrite = m_env.allOptionsMap()[m_option_chain_write.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_computeStats.c_str())) {
    m_chainComputeStats = m_env.allOptionsMap()[m_option_chain_computeStats.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_generateExtra.c_str())) {
    m_chainGenerateExtra = m_env.allOptionsMap()[m_option_chain_generateExtra.c_str()].as<bool>();
  }

  //if (m_env.allOptionsMap().count(m_option_uniqueChain_generate.c_str())) {
  //  m_uniqueChainGenerate = m_env.allOptionsMap()[m_option_uniqueChain_generate.c_str()].as<bool>();
  //}

  //if (m_env.allOptionsMap().count(m_option_uniqueChain_write.c_str())) {
  //  m_uniqueChainWrite = m_env.allOptionsMap()[m_option_uniqueChain_write.c_str()].as<bool>();
  //}

  //if (m_env.allOptionsMap().count(m_option_uniqueChain_computeStats.c_str())) {
  //  m_uniqueChainComputeStats = m_env.allOptionsMap()[m_option_uniqueChain_computeStats.c_str()].as<bool>();
  //}

  if (m_env.allOptionsMap().count(m_option_filteredChain_generate.c_str())) {
    m_filteredChainGenerate = m_env.allOptionsMap()[m_option_filteredChain_generate.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_filteredChain_discardedPortion.c_str())) {
    m_filteredChainDiscardedPortion = m_env.allOptionsMap()[m_option_filteredChain_discardedPortion.c_str()].as<double>();
  }

  if (m_env.allOptionsMap().count(m_option_filteredChain_lag.c_str())) {
    m_filteredChainLag = m_env.allOptionsMap()[m_option_filteredChain_lag.c_str()].as<unsigned int>();
  }

  if (m_env.allOptionsMap().count(m_option_filteredChain_write.c_str())) {
    m_filteredChainWrite = m_env.allOptionsMap()[m_option_filteredChain_write.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_filteredChain_computeStats.c_str())) {
    m_filteredChainComputeStats = m_env.allOptionsMap()[m_option_filteredChain_computeStats.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_chain_outputFileName.c_str())) {
    m_chainOutputFileName = m_env.allOptionsMap()[m_option_chain_outputFileName.c_str()].as<std::string>();
  }

  if (m_env.allOptionsMap().count(m_option_tk_useLocalHessian.c_str())) {
    m_tkUseLocalHessian = m_env.allOptionsMap()[m_option_tk_useLocalHessian.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_tk_useNewtonComponent.c_str())) {
    m_tkUseNewtonComponent = m_env.allOptionsMap()[m_option_tk_useNewtonComponent.c_str()].as<bool>();
  }

  if (m_env.allOptionsMap().count(m_option_dr_maxNumExtraStages.c_str())) {
    m_drMaxNumExtraStages = m_env.allOptionsMap()[m_option_dr_maxNumExtraStages.c_str()].as<unsigned int>();
  }

  std::vector<double> tmpScales(0,0.);
  if (m_env.allOptionsMap().count(m_option_dr_scalesForExtraStages.c_str())) {
    std::string inputString = m_env.allOptionsMap()[m_option_dr_scalesForExtraStages.c_str()].as<std::string>();
    uqMiscReadDoublesFromString(inputString,tmpScales);
    //std::cout << "In uqMarkovChainSGClass<P_V,P_M>::getMyOptionValues(): scales =";
    //for (unsigned int i = 0; i < tmpScales.size(); ++i) {
    //  std::cout << " " << tmpScales[i];
    //}
    //std::cout << std::endl;
  }

  if (m_drMaxNumExtraStages > 0) {
    m_drScalesForCovMatrices.clear();
#ifdef UQ_USES_TK_CLASS
#else
    m_lowerCholProposalCovMatrices.clear();
    m_proposalCovMatrices.clear();
#endif

    double scale = 1.0;
    unsigned int tmpSize = tmpScales.size();

    m_drScalesForCovMatrices.resize    (m_drMaxNumExtraStages+1,1.);
#ifdef UQ_USES_TK_CLASS
#else
    m_lowerCholProposalCovMatrices.resize(m_drMaxNumExtraStages+1,NULL);
    m_proposalCovMatrices.resize         (m_drMaxNumExtraStages+1,NULL);
#endif

    for (unsigned int i = 1; i < (m_drMaxNumExtraStages+1); ++i) {
      if (i <= tmpSize) scale = tmpScales[i-1];
      m_drScalesForCovMatrices[i] = scale;
    }
    //updateTK();
  }

  if (m_env.allOptionsMap().count(m_option_am_initialNonAdaptInterval.c_str())) {
    m_amInitialNonAdaptInterval = m_env.allOptionsMap()[m_option_am_initialNonAdaptInterval.c_str()].as<unsigned int>();
  }

  if (m_env.allOptionsMap().count(m_option_am_adaptInterval.c_str())) {
    m_amAdaptInterval = m_env.allOptionsMap()[m_option_am_adaptInterval.c_str()].as<unsigned int>();
  }

  if (m_env.allOptionsMap().count(m_option_am_eta.c_str())) {
    m_amEta = m_env.allOptionsMap()[m_option_am_eta.c_str()].as<double>();
  }

  if (m_env.allOptionsMap().count(m_option_am_epsilon.c_str())) {
    m_amEpsilon = m_env.allOptionsMap()[m_option_am_epsilon.c_str()].as<double>();
  }

  return;
}

#ifdef UQ_USES_TK_CLASS
#else
template<class P_V,class P_M>
int
uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()
{
//const P_M& proposalCovMatrix (*m_initialProposalCovMatrix);
//const P_M& proposalPrecMatrix(...);

  if ((m_env.verbosity() >= 5) && (m_env.rank() == 0)) {
    std::cout << "Entering uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()..."
              << std::endl;
  }

  int iRC = UQ_OK_RC;

  if (m_env.rank() == 0) std::cout << "In uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()"
                                   << ": using supplied initialProposalCovMatrix, whose contents are:"
                                   << std::endl;
  std::cout << *m_initialProposalCovMatrix;
  if (m_env.rank() == 0) std::cout << std::endl;

#ifdef UQ_USES_TK_CLASS
#else
  m_lowerCholProposalCovMatrices[0] = new P_M(*m_initialProposalCovMatrix); 
  iRC = m_lowerCholProposalCovMatrices[0]->chol();
  UQ_FATAL_RC_MACRO(iRC,
                    m_env.rank(),
                    "uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()",
                    "initialProposalCovMatrix is not positive definite");
  m_lowerCholProposalCovMatrices[0]->zeroUpper(false);

  m_proposalCovMatrices[0] = new P_M(*m_initialProposalCovMatrix);

  if (m_env.rank() == 0) std::cout << "In uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()"
                                   << ", m_lowerCholProposalCovMatrices[0] contents are:"
                                   << std::endl;
  std::cout << *(m_lowerCholProposalCovMatrices[0]);
  if (m_env.rank() == 0) std::cout << std::endl;

#ifdef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
  const P_M* internalProposalPrecMatrix = proposalPrecMatrix;
  if (proposalPrecMatrix == NULL) {
    UQ_FATAL_RC_MACRO(UQ_INCOMPLETE_IMPLEMENTATION_RC,
                      m_env.rank(),
                      "uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()",
                      "not yet implemented for the case 'proposalPrecMatrix == NULL'");
  }

  m_upperCholProposalPrecMatrices[0] = new P_M(proposalPrecMatrix); 
  iRC = m_upperCholProposalPrecMatrices[0]->chol();
  UQ_FATAL_RC_MACRO(iRC,
                    m_env.rank(),
                    "uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()",
                    "proposalPrecMatrix is not positive definite");
  m_upperCholProposalPrecMatrices[0]->zeroLower(false);

  m_proposalPrecMatrices[0] = new P_M(proposalPrecMatrix);

  //if (m_env.rank() == 0) std::cout << "In uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()"
  //                                 << ", m_upperCholProposalPrecMatrices[0] contents are:"
  //                                 << std::endl;
  //std::cout << *(m_upperCholProposalPrecMatrices[0]);
  //if (m_env.rank() == 0) std::cout << std::endl;
#endif
#endif

  if ((m_env.verbosity() >= 5) && (m_env.rank() == 0)) {
    std::cout << "Leaving uqMarkovChainSGClass<P_V,P_M>::computeInitialCholFactors()"
              << std::endl;
  }

  return iRC;
}

template<class P_V,class P_M>
void
uqMarkovChainSGClass<P_V,P_M>::updateTK()
{
  if ((m_env.verbosity() >= 5) && (m_env.rank() == 0)) {
    std::cout << "Entering uqMarkovChainSGClass<P_V,P_M>::updateTK()"
              << std::endl;
  }

  if ((m_env.verbosity() >= 5) && (m_env.rank() == 0)) {
    std::cout << "In uqMarkovChainSGClass<P_V,P_M>::updateTK()"
              << ": m_drMaxNumExtraStages = "                 << m_drMaxNumExtraStages
              << ", m_drScalesForCovMatrices.size() = "       << m_drScalesForCovMatrices.size()
#ifdef UQ_USES_TK_CLASS
#else
              << ", m_lowerCholProposalCovMatrices.size() = " << m_lowerCholProposalCovMatrices.size()
#endif
              << std::endl;
  }

#ifdef UQ_USES_TK_CLASS
#else
  for (unsigned int i = 1; i < (m_drMaxNumExtraStages+1); ++i) {
    double scale = m_drScalesForCovMatrices[i];
    if (m_lowerCholProposalCovMatrices[i]) delete m_lowerCholProposalCovMatrices[i];
    m_lowerCholProposalCovMatrices [i]   = new P_M(*(m_lowerCholProposalCovMatrices[i-1]));
  *(m_lowerCholProposalCovMatrices [i]) /= scale;
    if (m_proposalCovMatrices[i]) delete m_proposalCovMatrices[i];
    m_proposalCovMatrices[i]             = new P_M(*(m_proposalCovMatrices[i-1]));
  *(m_proposalCovMatrices[i])           /= (scale*scale);
#ifdef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
    m_upperCholProposalPrecMatrices[i]   = new P_M(*(m_upperCholProposalPrecMatrices[i-1]));
  *(m_upperCholProposalPrecMatrices[i]) *= scale;
    m_proposalPrecMatrices[i]            = new P_M(*(m_proposalPrecMatrices[i-1]));
  *(m_proposalPrecMatrices[i])          *= (scale*scale);
#endif
  }
#endif

  if ((m_env.verbosity() >= 5) && (m_env.rank() == 0)) {
    std::cout << "Leaving uqMarkovChainSGClass<P_V,P_M>::updateTK()"
              << std::endl;
  }

  return;
}

template<class P_V,class P_M>
double
uqMarkovChainSGClass<P_V,P_M>::logProposal(
  const uqMarkovChainPositionClass<P_V>& x,
  const uqMarkovChainPositionClass<P_V>& y,
  unsigned int                           idOfProposalCovMatrix)
{
#ifdef UQ_USES_TK_CLASS
  return 0.;
#else
  P_V diffVec(y.vecValues() - x.vecValues());
#ifdef UQ_MAC_SG_REQUIRES_INVERTED_COV_MATRICES
  double value = -0.5 * scalarProduct(diffVec, *(m_proposalPrecMatrices[idOfProposalCovMatrix]) * diffVec);
#else
  double value = -0.5 * scalarProduct(diffVec, m_proposalCovMatrices[idOfProposalCovMatrix]->invertMultiply(diffVec));
#endif
  return value;
#endif
}

template<class P_V,class P_M>
double
uqMarkovChainSGClass<P_V,P_M>::logProposal(const std::vector<uqMarkovChainPositionClass<P_V>*>& inputPositions)
{
  unsigned int inputSize = inputPositions.size();
  UQ_FATAL_TEST_MACRO((inputSize < 2),
                      m_env.rank(),
                      "uqMarkovChainSGClass<P_V,P_M>::logProposal()",
                      "inputPositions has size < 2");

  return this->logProposal(*(inputPositions[0            ]),
                           *(inputPositions[inputSize - 1]),
                           inputSize-2);
}
#endif // UQ_USES_TK_CLASS

template<class P_V,class P_M>
double
uqMarkovChainSGClass<P_V,P_M>::alpha(
  const uqMarkovChainPositionClass<P_V>& x,
  const uqMarkovChainPositionClass<P_V>& y,
  double*                                alphaQuotientPtr)
{
  double alphaQuotient = 0.;
  bool xOutOfBounds = x.outOfBounds();
  bool yOutOfBounds = y.outOfBounds();
  if ((xOutOfBounds == false) &&
      (yOutOfBounds == false)) {
    double yLogTargetToUse = y.logTarget();
#ifdef UQ_MAC_SG_REQUIRES_TARGET_DISTRIBUTION_ONLY
#else
    if (m_likelihoodObjComputesMisfits &&
        m_observableSpace.shouldVariancesBeUpdated()) {
      // Divide the misfitVector of 'y' by the misfitVarianceVector of 'x'
      yLogTargetToUse = -0.5 * ( y.m2lPrior() + (y.misfitVector()/x.misfitVarianceVector()).sumOfComponents() );
    }
#endif
    if (m_tkIsSymmetric) {
      alphaQuotient = exp(yLogTargetToUse - x.logTarget());
      if ((m_env.verbosity() >= 10) && (m_env.rank() == 0)) {
        std::cout << "In uqMarkovChainSGClass<P_V,P_M>::alpha()"
                  << ": symmetric proposal case"
                  << ", yLogTargetToUse = " << yLogTargetToUse
                  << ", x.logTarget() = "   << x.logTarget()
                  << ", alpha = "              << alphaQuotient
                  << std::endl;
      }
    }
    else {
#ifdef UQ_USES_TK_CLASS // AQUI
#else
      alphaQuotient = exp(yLogTargetToUse + logProposal(y,x,0) - x.logTarget() - logProposal(x,y,0));
#endif
    }
  }
  else {
    if ((m_env.verbosity() >= 10) && (m_env.rank() == 0)) {
      std::cout << "In uqMarkovChainSGClass<P_V,P_M>::alpha()"
                << ": xOutOfBounds = " << xOutOfBounds
                << ", yOutOfBounds = " << yOutOfBounds
                << std::endl;
    }
  }
  if (alphaQuotientPtr != NULL) *alphaQuotientPtr = alphaQuotient;

  return std::min(1.,alphaQuotient);
}

template<class P_V,class P_M>
double
uqMarkovChainSGClass<P_V,P_M>::alpha(const std::vector<uqMarkovChainPositionClass<P_V>*>& inputPositions)
{
  unsigned int inputSize = inputPositions.size();
  UQ_FATAL_TEST_MACRO((inputSize < 2),
                      m_env.rank(),
                      "uqMarkovChainSGClass<P_V,P_M>::alpha()",
                      "inputPositions has size < 2");

  // If necessary, return 0. right away
  if (inputPositions[0          ]->outOfBounds()) return 0.;
  if (inputPositions[inputSize-1]->outOfBounds()) return 0.;

  // If inputSize is 2, recursion is not needed
  if (inputSize == 2) return this->alpha(*(inputPositions[0            ]),
                                         *(inputPositions[inputSize - 1]));

  // Prepare two vectors of positions
  std::vector<uqMarkovChainPositionClass<P_V>*>         positions(inputSize,NULL);
  std::vector<uqMarkovChainPositionClass<P_V>*> backwardPositions(inputSize,NULL);
  for (unsigned int i = 0; i < inputSize; ++i) {
            positions[i] = inputPositions[i];
    backwardPositions[i] = inputPositions[inputSize-i-1];
  }

  // Initialize cumulative variables
  double logNumerator      = 0.;
  double logDenominator    = 0.;
  double alphasNumerator   = 1.;
  double alphasDenominator = 1.;

  // Compute cumulative variables
#ifdef UQ_USES_TK_CLASS // AQUI
#else
  logNumerator   += logProposal(backwardPositions);
  logDenominator += logProposal(        positions);
#endif

  for (unsigned int i = 0; i < (inputSize-2); ++i) { // That is why size must be >= 2
            positions.pop_back();
    backwardPositions.pop_back();

#ifdef UQ_USES_TK_CLASS // AQUI
#else
    logNumerator   += logProposal(backwardPositions);
    logDenominator += logProposal(        positions);
#endif

    alphasNumerator   *= (1 - this->alpha(backwardPositions));
    alphasDenominator *= (1 - this->alpha(        positions));
  }

  double numeratorLogTargetToUse = backwardPositions[0]->logTarget();
#ifdef UQ_MAC_SG_REQUIRES_TARGET_DISTRIBUTION_ONLY
#else
  if (m_likelihoodObjComputesMisfits &&
      m_observableSpace.shouldVariancesBeUpdated()) {
    // Divide the misfitVector of 'back[0]' by the misfitVarianceVector of 'pos[0]'
    numeratorLogTargetToUse = -0.5 * ( backwardPositions[0]->m2lPrior() +
      (backwardPositions[0]->misfitVector()/positions[0]->misfitVarianceVector()).sumOfComponents() );
  }
#endif
  logNumerator   += numeratorLogTargetToUse;
  logDenominator += positions[0]->logTarget();

  // Return result
  return std::min(1.,(alphasNumerator/alphasDenominator)*exp(logNumerator-logDenominator));
}

template<class P_V,class P_M>
bool
uqMarkovChainSGClass<P_V,P_M>::acceptAlpha(double alpha)
{
  bool result = false;

  if      (alpha <= 0.                          ) result = false;
  else if (alpha >= 1.                          ) result = true;
  else if (alpha >= gsl_rng_uniform(m_env.rng())) result = true;
  else                                            result = false;

  return result;
}

template<class P_V,class P_M>
int
uqMarkovChainSGClass<P_V,P_M>::writeInfo(
  const uqBaseVectorSequenceClass<P_V,P_M>& workingChain,
  std::ofstream&                        ofs) const
//const P_M*                   mahalanobisMatrix,
//bool                         applyMahalanobisInvert) const
{
  if (m_env.rank() == 0) {
    std::cout << "\n"
              << "\n-----------------------------------------------------"
              << "\n Writing extra information about the Markov chain " << workingChain.name() << " to output file ..."
              << "\n-----------------------------------------------------"
              << "\n"
              << std::endl;
  }

  int iRC = UQ_OK_RC;

  if (m_chainGenerateExtra) {
    // Write m_logTargets
    ofs << m_prefix << "logTargets = zeros(" << m_logTargets.size()
        << ","                                      << 1
        << ");"
        << std::endl;
    ofs << m_prefix << "logTargets = [";
    for (unsigned int i = 0; i < m_logTargets.size(); ++i) {
      ofs << m_logTargets[i]
          << std::endl;
    }
    ofs << "];\n";

    // Write m_alphaQuotients
    ofs << m_prefix << "alphaQuotients = zeros(" << m_alphaQuotients.size()
        << ","                                      << 1
        << ");"
        << std::endl;
    ofs << m_prefix << "alphaQuotients = [";
    for (unsigned int i = 0; i < m_alphaQuotients.size(); ++i) {
      ofs << m_alphaQuotients[i]
          << std::endl;
    }
    ofs << "];\n";
  }

  // Write names of components
  ofs << m_prefix << "componentNames = {";
  m_vectorSpace.printComponentsNames(ofs,false);
  ofs << "};\n";

#if 0
  // Write mahalanobis distances
  if (mahalanobisMatrix != NULL) {
    P_V diffVec(m_vectorSpace.zeroVector());
    ofs << m_prefix << "d = [";
    if (applyMahalanobisInvert) {
      P_V tmpVec(m_vectorSpace.zeroVector());
      P_V vec0(m_vectorSpace.zeroVector());
      workingChain.getPositionValues(0,vec0);
      for (unsigned int i = 0; i < workingChain.sequenceSize(); ++i) {
        workingChain.getPositionValues(i,tmpVec);
        diffVec = tmpVec - vec0;
        //diffVec = *(workingChain[i]) - *(workingChain[0]);
        ofs << scalarProduct(diffVec, mahalanobisMatrix->invertMultiply(diffVec))
            << std::endl;
      }
    }
    else {
      P_V tmpVec(m_vectorSpace.zeroVector());
      P_V vec0(m_vectorSpace.zeroVector());
      workingChain.getPositionValues(0,vec0);
      for (unsigned int i = 0; i < workingChain.sequenceSize(); ++i) {
        workingChain.getPositionValues(i,tmpVec);
        diffVec = tmpVec - vec0;
        //diffVec = *(workingChain[i]) - *(workingChain[0]);
        ofs << scalarProduct(diffVec, *mahalanobisMatrix * diffVec)
            << std::endl;
      }
    }
    ofs << "];\n";
  }
#endif

#if 0
  // Write prior mean values
  ofs << m_prefix << "priorMeanValues = ["
      << m_vectorSpace.priorMuValues()
      << "];\n";

  // Write prior sigma values
  ofs << m_prefix << "priorSigmaValues = ["
      << m_vectorSpace.priorSigmaValues()
      << "];\n";

#if 0
  ofs << m_prefix << "results.prior = [queso_priorMeanValues',queso_priorSigmaValues'];\n";
#endif

  // Write vector space lower bounds
  ofs << m_prefix << "minValues = ["
      << m_vectorSpace.minValues()
      << "];\n";

  // Write vector space upper bounds
  ofs << m_prefix << "maxValues = ["
      << m_vectorSpace.maxValues()
      << "];\n";
#endif

#if 0
  ofs << m_prefix << "results.limits = [queso_low',queso_upp'];\n";

  // Write out data for mcmcpred.m of MATLAB MCMC toolbox
  ofs << m_prefix << "results.parind = ["; // FIXME
  for (unsigned int i = 0; i < m_vectorSpace.dim(); ++i) {
    ofs << i+1
        << std::endl;
  }
  ofs << "];\n";

  ofs << m_prefix << "results.local = [\n"; // FIXME
  for (unsigned int i = 0; i < m_vectorSpace.dim(); ++i) {
    ofs << " 0";
    //<< std::endl;
  }
  ofs << "];\n";

  if (m_chainUse2) {
  }
  else {
    bool savedVectorPrintState = workingChain[workingChain.sequenceSize()-1]->getPrintHorizontally();
    workingChain[workingChain.sequenceSize()-1]->setPrintHorizontally(false);
    ofs << m_prefix << "results.theta = ["
        << *(workingChain[workingChain.sequenceSize()-1])
        << "];\n";
    workingChain[workingChain.sequenceSize()-1]->setPrintHorizontally(savedVectorPrintState);
  }
  
  ofs << m_prefix << "results.nbatch = 1.;\n"; // FIXME

  if (mahalanobisMatrix != NULL) {
    // Write covMatrix
    ofs << m_prefix << "mahalanobisMatrix = ["
        << *mahalanobisMatrix
        << "];\n";
  }
#endif

  // Write number of rejections
  ofs << m_prefix << "rejected = " << (double) m_numRejections/(double) (workingChain.sequenceSize()-1)
      << ";\n"
      << std::endl;

  // Write number of outbounds
  ofs << m_prefix << "outbounds = " << (double) m_numOutOfBounds/(double) (workingChain.sequenceSize()-1)
      << ";\n"
      << std::endl;

  // Write chain run time
  ofs << m_prefix << "runTime = " << m_chainRunTime
      << ";\n"
      << std::endl;

  if (m_env.rank() == 0) {
    std::cout << "\n-----------------------------------------------------"
              << "\n Finished writing extra information about the Markov chain " << workingChain.name()
              << "\n-----------------------------------------------------"
              << "\n"
              << std::endl;
  }

  return iRC;
}

template<class P_V,class P_M>
void
uqMarkovChainSGClass<P_V,P_M>::print(std::ostream& os) const
{
  os <<         m_option_chain_type            << " = " << m_chainType
     << "\n" << m_option_chain_size            << " = " << m_chainSize
     << "\n" << m_option_chain_outputFileName  << " = " << m_chainOutputFileName
     << "\n" << m_option_chain_generateExtra   << " = " << m_chainGenerateExtra
     << "\n" << m_option_chain_displayPeriod   << " = " << m_chainDisplayPeriod
     << "\n" << m_option_chain_measureRunTimes << " = " << m_chainMeasureRunTimes
     << "\n" << m_option_chain_write           << " = " << m_chainWrite
     << "\n" << m_option_chain_computeStats    << " = " << m_chainComputeStats;
//os << "\n" << m_option_uniqueChain_generate           << " = " << m_uniqueChainGenerate
//   << "\n" << m_option_uniqueChain_write              << " = " << m_uniqueChainWrite
//   << "\n" << m_option_uniqueChain_computeStats       << " = " << m_uniqueChainComputeStats
  os << "\n" << m_option_filteredChain_generate         << " = " << m_filteredChainGenerate
     << "\n" << m_option_filteredChain_discardedPortion << " = " << m_filteredChainDiscardedPortion
     << "\n" << m_option_filteredChain_lag              << " = " << m_filteredChainLag
     << "\n" << m_option_filteredChain_write            << " = " << m_filteredChainWrite
     << "\n" << m_option_filteredChain_computeStats     << " = " << m_filteredChainComputeStats
     << "\n" << m_option_tk_useLocalHessian             << " = " << m_tkUseLocalHessian
     << "\n" << m_option_tk_useNewtonComponent          << " = " << m_tkUseNewtonComponent
     << "\n" << m_option_dr_maxNumExtraStages           << " = " << m_drMaxNumExtraStages
     << "\n" << m_option_dr_scalesForExtraStages        << " = ";
  for (unsigned int i = 0; i < m_drScalesForCovMatrices.size(); ++i) {
    os << m_drScalesForCovMatrices[i] << " ";
  }
  os << "\n" << m_option_am_initialNonAdaptInterval << " = " << m_amInitialNonAdaptInterval
     << "\n" << m_option_am_adaptInterval           << " = " << m_amAdaptInterval
     << "\n" << m_option_am_eta                     << " = " << m_amEta
     << "\n" << m_option_am_epsilon                 << " = " << m_amEpsilon
     << std::endl;

  return;
}

template<class P_V,class P_M>
std::ostream& operator<<(std::ostream& os, const uqMarkovChainSGClass<P_V,P_M>& obj)
{
  obj.print(os);

  return os;
}
#endif // __UQ_MAC_SG1_H__