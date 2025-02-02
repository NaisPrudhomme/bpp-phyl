//
// File: OptimizationTools.cpp
// Authors:
//   Julien Dutheil
// Created: 2003-12-14 09:43:32
//

/*
  Copyright or ÃÂ© or Copr. Bio++ Development Team, (November 16, 2004)
  
  This software is a computer program whose purpose is to provide classes
  for phylogenetic data analysis.
  
  This software is governed by the CeCILL license under French law and
  abiding by the rules of distribution of free software. You can use,
  modify and/ or redistribute the software under the terms of the CeCILL
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info".
  
  As a counterpart to the access to the source code and rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty and the software's author, the holder of the
  economic rights, and the successive licensors have only limited
  liability.
  
  In this respect, the user's attention is drawn to the risks associated
  with loading, using, modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean that it is complicated to manipulate, and that also
  therefore means that it is reserved for developers and experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or
  data to be ensured and, more generally, to use and operate it in the
  same conditions as regards security.
  
  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL license and that you accept its terms.
*/

#include <Bpp/App/ApplicationTools.h>
#include <Bpp/Numeric/Function/BfgsMultiDimensions.h>
#include <Bpp/Numeric/Function/ConjugateGradientMultiDimensions.h>
#include <Bpp/Numeric/Function/DownhillSimplexMethod.h>
#include <Bpp/Numeric/Function/ReparametrizationFunctionWrapper.h>
#include <Bpp/Numeric/Function/ThreePointsNumericalDerivative.h>
#include <Bpp/Numeric/Function/TwoPointsNumericalDerivative.h>
#include <Bpp/Numeric/ParameterList.h>

#include "../Io/Newick.h"
#include "../PseudoNewtonOptimizer.h"
#include "../Tree/NNISearchable.h"
#include "Likelihood/GlobalClockTreeLikelihoodFunctionWrapper.h"
#include "OptimizationTools.h"
#include "Tree/NNITopologySearch.h"

// From bpp-seq:
#include <Bpp/Seq/Io/Fasta.h>

using namespace bpp;
using namespace std;

/******************************************************************************/

OptimizationToolsOld::OptimizationToolsOld() {}

OptimizationToolsOld::~OptimizationToolsOld() {}

/******************************************************************************/

OptimizationToolsOld::ScaleFunction::ScaleFunction(TreeLikelihood* tl) :
  tl_(tl),
  brLen_(),
  lambda_()
{
  // We work only on the branch lengths:
  brLen_ = tl->getBranchLengthsParameters();
  if (brLen_.hasParameter("RootPosition"))
    brLen_.deleteParameter("RootPosition");
  lambda_.addParameter(Parameter("scale factor", 0));
}

OptimizationToolsOld::ScaleFunction::~ScaleFunction() {}

void OptimizationToolsOld::ScaleFunction::setParameters(const ParameterList& lambda)
{
  if (lambda.size() != 1)
    throw Exception("OptimizationToolsOld::ScaleFunction::f(). This is a one parameter function!");
  lambda_.setParametersValues(lambda);
}

double OptimizationToolsOld::ScaleFunction::getValue() const
{
  // Scale the tree:
  ParameterList brLen = brLen_;
  double s = exp(lambda_[0].getValue());
  for (unsigned int i = 0; i < brLen.size(); i++)
  {
    try
    {
      brLen[i].setValue(brLen[i].getValue() * s);
    }
    catch (ConstraintException& cex)
    {
      // Do nothing. Branch value is already at bound...
    }
  }
  return tl_->f(brLen);
}

/******************************************************************************/

unsigned int OptimizationToolsOld::optimizeTreeScale(
  TreeLikelihood* tl,
  double tolerance,
  unsigned int tlEvalMax,
  OutputStream* messageHandler,
  OutputStream* profiler,
  unsigned int verbose)
{
  ScaleFunction sf(tl);
  BrentOneDimension bod(&sf);
  bod.setMessageHandler(messageHandler);
  bod.setProfiler(profiler);
  ParameterList singleParameter;
  singleParameter.addParameter(Parameter("scale factor", 0));
  bod.setInitialInterval(-0.5, 0.5);
  bod.init(singleParameter);
  ParametersStopCondition PS(&bod, tolerance);
  bod.setStopCondition(PS);
  bod.setMaximumNumberOfEvaluations(tlEvalMax);
  bod.optimize();
  ApplicationTools::displayTaskDone();
  if (verbose > 0)
    ApplicationTools::displayResult("Tree scaled by", exp(sf.getParameters()[0].getValue()));
  return bod.getNumberOfEvaluations();
}

/******************************************************************************/

unsigned int OptimizationToolsOld::optimizeNumericalParameters(
  DiscreteRatesAcrossSitesTreeLikelihood* tl,
  const ParameterList& parameters,
  OptimizationListener* listener,
  unsigned int nstep,
  double tolerance,
  unsigned int tlEvalMax,
  OutputStream* messageHandler,
  OutputStream* profiler,
  bool reparametrization,
  unsigned int verbose,
  const std::string& optMethodDeriv,
  const std::string& optMethodModel)
{
  DerivableSecondOrder* f = tl;
  ParameterList pl = parameters;

  // Shall we reparametrize the function to remove constraints?
  unique_ptr<DerivableSecondOrder> frep;
  if (reparametrization)
  {
    frep.reset(new ReparametrizationDerivableSecondOrderWrapper(f, parameters));
    f = frep.get();

    // Reset parameters to remove constraints:
    pl = f->getParameters().createSubList(parameters.getParameterNames());
  }

  // ///////////////
  // Build optimizer:

  // Branch lengths

  MetaOptimizerInfos* desc = new MetaOptimizerInfos();
  MetaOptimizer* poptimizer = 0;
  unique_ptr<AbstractNumericalDerivative> fnum(new ThreePointsNumericalDerivative(f));

  if (optMethodDeriv == OPTIMIZATION_GRADIENT)
    desc->addOptimizer("Branch length parameters", new ConjugateGradientMultiDimensions(f), tl->getBranchLengthsParameters().getParameterNames(), 2, MetaOptimizerInfos::IT_TYPE_FULL);
  else if (optMethodDeriv == OPTIMIZATION_NEWTON)
    desc->addOptimizer("Branch length parameters", new PseudoNewtonOptimizer(f), tl->getBranchLengthsParameters().getParameterNames(), 2, MetaOptimizerInfos::IT_TYPE_FULL);
  else if (optMethodDeriv == OPTIMIZATION_BFGS)
    desc->addOptimizer("Branch length parameters", new BfgsMultiDimensions(f), tl->getBranchLengthsParameters().getParameterNames(), 2, MetaOptimizerInfos::IT_TYPE_FULL);
  else
    throw Exception("OptimizationToolsOld::optimizeNumericalParameters. Unknown optimization method: " + optMethodDeriv);

  // Other parameters

  if (optMethodModel == OPTIMIZATION_BRENT)
  {
    ParameterList plsm = parameters.getCommonParametersWith(tl->getSubstitutionModelParameters());
    desc->addOptimizer("Substitution model parameter", new SimpleMultiDimensions(f), plsm.getParameterNames(), 0, MetaOptimizerInfos::IT_TYPE_STEP);


    ParameterList plrd = parameters.getCommonParametersWith(tl->getRateDistributionParameters());
    desc->addOptimizer("Rate distribution parameter", new SimpleMultiDimensions(f), plrd.getParameterNames(), 0, MetaOptimizerInfos::IT_TYPE_STEP);
    poptimizer = new MetaOptimizer(f, desc, nstep);
  }
  else if (optMethodModel == OPTIMIZATION_BFGS)
  {
    vector<string> vNameDer;

    ParameterList plsm = parameters.getCommonParametersWith(tl->getSubstitutionModelParameters());
    vNameDer = plsm.getParameterNames();

    ParameterList plrd = parameters.getCommonParametersWith(tl->getRateDistributionParameters());

    vector<string> vNameDer2 = plrd.getParameterNames();

    vNameDer.insert(vNameDer.begin(), vNameDer2.begin(), vNameDer2.end());
    fnum->setParametersToDerivate(vNameDer);

    desc->addOptimizer("Rate & model distribution parameters", new BfgsMultiDimensions(fnum.get()), vNameDer, 1, MetaOptimizerInfos::IT_TYPE_FULL);
    poptimizer = new MetaOptimizer(fnum.get(), desc, nstep);
  }
  else
    throw Exception("OptimizationToolsOld::optimizeNumericalParameters. Unknown optimization method: " + optMethodModel);

  poptimizer->setVerbose(verbose);
  poptimizer->setProfiler(profiler);
  poptimizer->setMessageHandler(messageHandler);
  poptimizer->setMaximumNumberOfEvaluations(tlEvalMax);
  poptimizer->getStopCondition()->setTolerance(tolerance);

  // Optimize TreeLikelihood function:
  poptimizer->setConstraintPolicy(AutoParameter::CONSTRAINTS_AUTO);
  NaNListener* nanListener = new NaNListener(poptimizer, tl);
  poptimizer->addOptimizationListener(nanListener);
  if (listener)
    poptimizer->addOptimizationListener(listener);
  poptimizer->init(pl);
  poptimizer->optimize();

  if (verbose > 0)
    ApplicationTools::displayMessage("\n");

  // We're done.
  unsigned int nb = poptimizer->getNumberOfEvaluations();
  delete poptimizer;
  delete nanListener;
  return nb;
}


/******************************************************************************/

unsigned int OptimizationToolsOld::optimizeNumericalParameters2(
  DiscreteRatesAcrossSitesTreeLikelihood* tl,
  const ParameterList& parameters,
  OptimizationListener* listener,
  double tolerance,
  unsigned int tlEvalMax,
  OutputStream* messageHandler,
  OutputStream* profiler,
  bool reparametrization,
  bool useClock,
  unsigned int verbose,
  const std::string& optMethodDeriv)
{
  DerivableSecondOrder* f = tl;
  ParameterList pl = parameters;
  // Shall we use a molecular clock constraint on branch lengths?
  unique_ptr<GlobalClockTreeLikelihoodFunctionWrapper> fclock;
  if (useClock)
  {
    fclock.reset(new GlobalClockTreeLikelihoodFunctionWrapper(tl));
    f = fclock.get();
    if (verbose > 0)
      ApplicationTools::displayResult("Log-likelihood after adding clock", -tl->getLogLikelihood());

    // Reset parameters to use new branch lengths. WARNING! 'old' branch parameters do not exist anymore and have been replaced by heights
    pl = fclock->getParameters().getCommonParametersWith(parameters);
    pl.addParameters(fclock->getHeightParameters());
  }
  // Shall we reparametrize the function to remove constraints?
  unique_ptr<DerivableSecondOrder> frep;
  if (reparametrization)
  {
    frep.reset(new ReparametrizationDerivableSecondOrderWrapper(f, pl));
    f = frep.get();

    // Reset parameters to remove constraints:
    pl = f->getParameters().createSubList(pl.getParameterNames());
  }

  unique_ptr<AbstractNumericalDerivative> fnum;
  // Build optimizer:
  unique_ptr<Optimizer> optimizer;
  if (optMethodDeriv == OPTIMIZATION_GRADIENT)
  {
    fnum.reset(new TwoPointsNumericalDerivative(f));
    fnum->setInterval(0.0000001);
    optimizer.reset(new ConjugateGradientMultiDimensions(fnum.get()));
  }
  else if (optMethodDeriv == OPTIMIZATION_NEWTON)
  {
    fnum.reset(new ThreePointsNumericalDerivative(f));
    fnum->setInterval(0.0001);
    optimizer.reset(new PseudoNewtonOptimizer(fnum.get()));
  }
  else if (optMethodDeriv == OPTIMIZATION_BFGS)
  {
    fnum.reset(new TwoPointsNumericalDerivative(f));
    fnum->setInterval(0.0001);
    optimizer.reset(new BfgsMultiDimensions(fnum.get()));
  }
  else
    throw Exception("OptimizationToolsOld::optimizeNumericalParameters2. Unknown optimization method: " + optMethodDeriv);

  // Numerical derivatives:
  ParameterList tmp = tl->getNonDerivableParameters();

  if (useClock)
    tmp.addParameters(fclock->getHeightParameters());
  fnum->setParametersToDerivate(tmp.getParameterNames());
  optimizer->setVerbose(verbose);
  optimizer->setProfiler(profiler);
  optimizer->setMessageHandler(messageHandler);
  optimizer->setMaximumNumberOfEvaluations(tlEvalMax);
  optimizer->getStopCondition()->setTolerance(tolerance);

  // Optimize TreeLikelihood function:
  optimizer->setConstraintPolicy(AutoParameter::CONSTRAINTS_AUTO);
  NaNListener* nanListener = new NaNListener(optimizer.get(), tl);
  optimizer->addOptimizationListener(nanListener);
  if (listener)
    optimizer->addOptimizationListener(listener);
  optimizer->init(pl);
  optimizer->optimize();

  if (verbose > 0)
    ApplicationTools::displayMessage("\n");

  // We're done.
  return optimizer->getNumberOfEvaluations();
}


/******************************************************************************/


unsigned int OptimizationToolsOld::optimizeBranchLengthsParameters(
  DiscreteRatesAcrossSitesTreeLikelihood* tl,
  const ParameterList& parameters,
  OptimizationListener* listener,
  double tolerance,
  unsigned int tlEvalMax,
  OutputStream* messageHandler,
  OutputStream* profiler,
  unsigned int verbose,
  const std::string& optMethodDeriv)
{
  // Build optimizer:
  Optimizer* optimizer = 0;
  if (optMethodDeriv == OPTIMIZATION_GRADIENT)
  {
    tl->enableFirstOrderDerivatives(true);
    tl->enableSecondOrderDerivatives(false);
    optimizer = new ConjugateGradientMultiDimensions(tl);
  }
  else if (optMethodDeriv == OPTIMIZATION_NEWTON)
  {
    tl->enableFirstOrderDerivatives(true);
    tl->enableSecondOrderDerivatives(true);
    optimizer = new PseudoNewtonOptimizer(tl);
  }
  else if (optMethodDeriv == OPTIMIZATION_BFGS)
  {
    tl->enableFirstOrderDerivatives(true);
    tl->enableSecondOrderDerivatives(false);
    optimizer = new BfgsMultiDimensions(tl);
  }
  else
    throw Exception("OptimizationToolsOld::optimizeBranchLengthsParameters. Unknown optimization method: " + optMethodDeriv);
  optimizer->setVerbose(verbose);
  optimizer->setProfiler(profiler);
  optimizer->setMessageHandler(messageHandler);
  optimizer->setMaximumNumberOfEvaluations(tlEvalMax);
  optimizer->getStopCondition()->setTolerance(tolerance);

  // Optimize TreeLikelihood function:
  ParameterList pl = parameters.getCommonParametersWith(tl->getBranchLengthsParameters());
  optimizer->setConstraintPolicy(AutoParameter::CONSTRAINTS_AUTO);
  if (listener)
    optimizer->addOptimizationListener(listener);
  optimizer->init(pl);
  optimizer->optimize();
  if (verbose > 0)
    ApplicationTools::displayMessage("\n");

  // We're done.
  unsigned int n = optimizer->getNumberOfEvaluations();
  delete optimizer;
  return n;
}

/******************************************************************************/

unsigned int OptimizationToolsOld::optimizeNumericalParametersWithGlobalClock(
  DiscreteRatesAcrossSitesClockTreeLikelihood* cl,
  const ParameterList& parameters,
  OptimizationListener* listener,
  unsigned int nstep,
  double tolerance,
  unsigned int tlEvalMax,
  OutputStream* messageHandler,
  OutputStream* profiler,
  unsigned int verbose,
  const std::string& optMethodDeriv)
{
  AbstractNumericalDerivative* fun = 0;

  // Build optimizer:
  MetaOptimizerInfos* desc = new MetaOptimizerInfos();
  if (optMethodDeriv == OPTIMIZATION_GRADIENT)
  {
    fun = new TwoPointsNumericalDerivative(cl);
    fun->setInterval(0.0000001);
    desc->addOptimizer("Branch length parameters", new ConjugateGradientMultiDimensions(fun), cl->getBranchLengthsParameters().getParameterNames(), 2, MetaOptimizerInfos::IT_TYPE_FULL);
  }
  else if (optMethodDeriv == OPTIMIZATION_NEWTON)
  {
    fun = new ThreePointsNumericalDerivative(cl);
    fun->setInterval(0.0001);
    desc->addOptimizer("Branch length parameters", new PseudoNewtonOptimizer(fun), cl->getBranchLengthsParameters().getParameterNames(), 2, MetaOptimizerInfos::IT_TYPE_FULL);
  }
  else
    throw Exception("OptimizationToolsOld::optimizeNumericalParametersWithGlobalClock. Unknown optimization method: " + optMethodDeriv);

  // Numerical derivatives:
  ParameterList tmp = parameters.getCommonParametersWith(cl->getBranchLengthsParameters());
  fun->setParametersToDerivate(tmp.getParameterNames());

  ParameterList plsm = parameters.getCommonParametersWith(cl->getSubstitutionModelParameters());
  if (plsm.size() < 10)
    desc->addOptimizer("Substitution model parameter", new SimpleMultiDimensions(cl), plsm.getParameterNames(), 0, MetaOptimizerInfos::IT_TYPE_STEP);
  else
    desc->addOptimizer("Substitution model parameters", new DownhillSimplexMethod(cl), plsm.getParameterNames(), 0, MetaOptimizerInfos::IT_TYPE_FULL);

  ParameterList plrd = parameters.getCommonParametersWith(cl->getRateDistributionParameters());
  if (plrd.size() < 10)
    desc->addOptimizer("Rate distribution parameter", new SimpleMultiDimensions(cl), plrd.getParameterNames(), 0, MetaOptimizerInfos::IT_TYPE_STEP);
  else
    desc->addOptimizer("Rate dsitribution parameters", new DownhillSimplexMethod(cl), plrd.getParameterNames(), 0, MetaOptimizerInfos::IT_TYPE_FULL);

  MetaOptimizer optimizer(fun, desc, nstep);
  optimizer.setVerbose(verbose);
  optimizer.setProfiler(profiler);
  optimizer.setMessageHandler(messageHandler);
  optimizer.setMaximumNumberOfEvaluations(tlEvalMax);
  optimizer.getStopCondition()->setTolerance(tolerance);

  // Optimize TreeLikelihood function:
  optimizer.setConstraintPolicy(AutoParameter::CONSTRAINTS_AUTO);
  if (listener)
    optimizer.addOptimizationListener(listener);
  optimizer.init(parameters);
  optimizer.optimize();
  if (verbose > 0)
    ApplicationTools::displayMessage("\n");

  // We're done.
  return optimizer.getNumberOfEvaluations();
}

/******************************************************************************/

unsigned int OptimizationToolsOld::optimizeNumericalParametersWithGlobalClock2(
  DiscreteRatesAcrossSitesClockTreeLikelihood* cl,
  const ParameterList& parameters,
  OptimizationListener* listener,
  double tolerance,
  unsigned int tlEvalMax,
  OutputStream* messageHandler,
  OutputStream* profiler,
  unsigned int verbose,
  const std::string& optMethodDeriv)
{
  AbstractNumericalDerivative* fun = 0;

  // Build optimizer:
  Optimizer* optimizer = 0;
  if (optMethodDeriv == OPTIMIZATION_GRADIENT)
  {
    fun = new TwoPointsNumericalDerivative(cl);
    fun->setInterval(0.0000001);
    optimizer = new ConjugateGradientMultiDimensions(fun);
  }
  else if (optMethodDeriv == OPTIMIZATION_NEWTON)
  {
    fun = new ThreePointsNumericalDerivative(cl);
    fun->setInterval(0.0001);
    optimizer = new PseudoNewtonOptimizer(fun);
  }
  else
    throw Exception("OptimizationToolsOld::optimizeBranchLengthsParameters. Unknown optimization method: " + optMethodDeriv);

  // Numerical derivatives:
  ParameterList tmp = parameters.getCommonParametersWith(cl->getParameters());
  fun->setParametersToDerivate(tmp.getParameterNames());

  optimizer->setVerbose(verbose);
  optimizer->setProfiler(profiler);
  optimizer->setMessageHandler(messageHandler);
  optimizer->setMaximumNumberOfEvaluations(tlEvalMax);
  optimizer->getStopCondition()->setTolerance(tolerance);

  // Optimize TreeLikelihood function:
  optimizer->setConstraintPolicy(AutoParameter::CONSTRAINTS_AUTO);
  if (listener)
    optimizer->addOptimizationListener(listener);
  optimizer->init(parameters);
  optimizer->optimize();
  if (verbose > 0)
    ApplicationTools::displayMessage("\n");

  // We're done.
  unsigned int n = optimizer->getNumberOfEvaluations();
  delete optimizer;

  // We're done.
  return n;
}

/******************************************************************************/

void NNITopologyListener::topologyChangeSuccessful(const TopologyChangeEvent& event)
{
  optimizeCounter_++;
  if (optimizeCounter_ == optimizeNumerical_)
  {
    DiscreteRatesAcrossSitesTreeLikelihood* likelihood = dynamic_cast<DiscreteRatesAcrossSitesTreeLikelihood*>(topoSearch_->getSearchableObject());
    parameters_.matchParametersValues(likelihood->getParameters());
    OptimizationToolsOld::optimizeNumericalParameters(likelihood, parameters_, 0, nStep_, tolerance_, 1000000, messenger_, profiler_, reparametrization_, verbose_, optMethod_);
    optimizeCounter_ = 0;
  }
}

/******************************************************************************/

void NNITopologyListener2::topologyChangeSuccessful(const TopologyChangeEvent& event)
{
  optimizeCounter_++;
  if (optimizeCounter_ == optimizeNumerical_)
  {
    DiscreteRatesAcrossSitesTreeLikelihood* likelihood = dynamic_cast<DiscreteRatesAcrossSitesTreeLikelihood*>(topoSearch_->getSearchableObject());
    parameters_.matchParametersValues(likelihood->getParameters());
    OptimizationToolsOld::optimizeNumericalParameters2(likelihood, parameters_, 0, tolerance_, 1000000, messenger_, profiler_, reparametrization_, false, verbose_, optMethod_);
    optimizeCounter_ = 0;
  }
}

// ******************************************************************************/

NNIHomogeneousTreeLikelihood* OptimizationToolsOld::optimizeTreeNNI(
  NNIHomogeneousTreeLikelihood* tl,
  const ParameterList& parameters,
  bool optimizeNumFirst,
  double tolBefore,
  double tolDuring,
  unsigned int tlEvalMax,
  unsigned int numStep,
  OutputStream* messageHandler,
  OutputStream* profiler,
  bool reparametrization,
  unsigned int verbose,
  const std::string& optMethodDeriv,
  unsigned int nStep,
  const std::string& nniMethod)
{
  // Roughly optimize parameter
  if (optimizeNumFirst)
  {
    OptimizationToolsOld::optimizeNumericalParameters(tl, parameters, NULL, nStep, tolBefore, 1000000, messageHandler, profiler, reparametrization, verbose, optMethodDeriv);
  }
  // Begin topo search:
  NNITopologySearch topoSearch(*tl, nniMethod, verbose > 2 ? verbose - 2 : 0);
  NNITopologyListener* topoListener = new NNITopologyListener(&topoSearch, parameters, tolDuring, messageHandler, profiler, verbose, optMethodDeriv, nStep, reparametrization);
  topoListener->setNumericalOptimizationCounter(numStep);
  topoSearch.addTopologyListener(topoListener);
  topoSearch.search();
  return dynamic_cast<NNIHomogeneousTreeLikelihood*>(topoSearch.getSearchableObject());
}

/******************************************************************************/

NNIHomogeneousTreeLikelihood* OptimizationToolsOld::optimizeTreeNNI2(
  NNIHomogeneousTreeLikelihood* tl,
  const ParameterList& parameters,
  bool optimizeNumFirst,
  double tolBefore,
  double tolDuring,
  unsigned int tlEvalMax,
  unsigned int numStep,
  OutputStream* messageHandler,
  OutputStream* profiler,
  bool reparametrization,
  unsigned int verbose,
  const std::string& optMethodDeriv,
  const std::string& nniMethod)
{
  // Roughly optimize parameter
  if (optimizeNumFirst)
  {
    OptimizationToolsOld::optimizeNumericalParameters2(tl, parameters, NULL, tolBefore, 1000000, messageHandler, profiler, reparametrization, false, verbose, optMethodDeriv);
  }
  // Begin topo search:
  NNITopologySearch topoSearch(*tl, nniMethod, verbose > 2 ? verbose - 2 : 0);
  NNITopologyListener2* topoListener = new NNITopologyListener2(&topoSearch, parameters, tolDuring, messageHandler, profiler, verbose, optMethodDeriv, reparametrization);
  topoListener->setNumericalOptimizationCounter(numStep);
  topoSearch.addTopologyListener(topoListener);
  topoSearch.search();
  return dynamic_cast<NNIHomogeneousTreeLikelihood*>(topoSearch.getSearchableObject());
}

/******************************************************************************/

DRTreeParsimonyScore* OptimizationToolsOld::optimizeTreeNNI(
  DRTreeParsimonyScore* tp,
  unsigned int verbose)
{
  NNISearchable* topo = dynamic_cast<NNISearchable*>(tp);
  NNITopologySearch topoSearch(*topo, NNITopologySearch::PHYML, verbose);
  topoSearch.search();
  return dynamic_cast<DRTreeParsimonyScore*>(topoSearch.getSearchableObject());
}


