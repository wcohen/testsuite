/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

// $Id: test1_19.C,v 1.1 2008/10/30 19:18:08 legendre Exp $
/*
 * #Name: test1_19
 * #Desc: Mutator Side - oneTimeCode
 * #Dep: 
 * #Notes: This test needs to be able to manipulate control flow
 */

#include "BPatch.h"
#include "BPatch_Vector.h"
#include "BPatch_thread.h"
#include "BPatch_snippet.h"

#include "test_lib.h"

#include "dyninst_comp.h"

class test1_19_Mutator : public DyninstMutator {

  virtual bool hasCustomExecutionPath() { return true; }
  virtual test_results_t executeTest();
};

extern "C" DLLEXPORT  TestMutator *test1_19_factory() 
{
  return new test1_19_Mutator();
}

static void test19_oneTimeCodeCallback(BPatch_thread * /*thread*/,
				void *userData,
				void * /*returnValue*/)
{
   *(int *)userData = 1;
}

//
// Start Test Case #19 - mutator side (oneTimeCode)
//

test_results_t test1_19_Mutator::executeTest() 
{
    // Avoid a race condition in fast & loose mode

    while (!appThread->isStopped()) 
	{
		BPatch::bpatch->waitForStatusChange();
    }

    appThread->continueExecution();

    if (waitUntilStopped(BPatch::bpatch, appThread, 19, "oneTimeCode") < 0) 
	{
      return FAILED;
    }

    BPatch_Vector<BPatch_function *> bpfv;
    char *fn = "test1_19_call1";

    if (NULL == appImage->findFunction(fn, bpfv) || !bpfv.size()
	|| NULL == bpfv[0])
	{
      logerror("    Unable to find function %s\n", fn);
      return FAILED;
    }

    BPatch_function *call19_1_func = bpfv[0];

    BPatch_Vector<BPatch_snippet *> nullArgs;
    BPatch_funcCallExpr call19_1Expr(*call19_1_func, nullArgs);
    checkCost(call19_1Expr);

    appThread->oneTimeCode(call19_1Expr);

    // Let the mutatee run to check the result
    appThread->continueExecution();

    // Wait for the next test

    if (waitUntilStopped(BPatch::bpatch, appThread, 19, "oneTimeCode") < 0) 
	{
      return FAILED;
    }

    bpfv.clear();
    char *fn2 = "test1_19_call2";

    if (NULL == appImage->findFunction(fn2, bpfv) || !bpfv.size()
	|| NULL == bpfv[0])
	{
      logerror("    Unable to find function %s\n", fn2);
      return FAILED;
    }

    BPatch_function *call19_2_func = bpfv[0];

    BPatch_funcCallExpr call19_2Expr(*call19_2_func, nullArgs);
    checkCost(call19_2Expr);

    int callbackFlag = 0;

    // Register a callback that will set the flag callbackFlag
    BPatchOneTimeCodeCallback oldCallback = 
       BPatch::bpatch->registerOneTimeCodeCallback(test19_oneTimeCodeCallback);

    appThread->oneTimeCodeAsync(call19_2Expr, (void *)&callbackFlag);

    while (!appThread->isTerminated() && !appThread->isStopped() )
    {
		BPatch::bpatch->waitForStatusChange();
    }
    
    // Continue mutatee after one-time code runs
    appThread->continueExecution();

    // Wait for the callback to be called
    while (!appThread->isTerminated() && !callbackFlag) ;

    // Restore old callback (if there was one)
	BPatch::bpatch->registerOneTimeCodeCallback(oldCallback);

    return PASSED;
} // test1_19_Mutator::executeTest()
