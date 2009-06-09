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

// $Id: test1_12.C,v 1.1 2008/10/30 19:17:33 legendre Exp $
/*
 * #Name: test1_12
 * #Desc: Mutator Side - Insert/Remove and Malloc/Free
 * #Dep: 
 * #Notes: 
 */

#include "BPatch.h"
#include "BPatch_Vector.h"
#include "BPatch_thread.h"
#include "BPatch_snippet.h"

#include "test_lib.h"
#include "Callbacks.h"
#include "dyninst_comp.h"

class test1_12_Mutator : public DyninstMutator 
{
	static const int HEAP_TEST_UNIT_SIZE = 5000;

	BPatchSnippetHandle *snippetHandle12_1;
	BPatch_variableExpr *varExpr12_1;

	virtual bool hasCustomExecutionPath() { return true; }
	virtual test_results_t executeTest();

	int mutatorTesta();
	int mutatorTestb();
};

extern "C" DLLEXPORT  TestMutator *test1_12_factory() 
{
	return new test1_12_Mutator();
}

//
// Start Test Case #12 - mutator side (insert/remove and malloc/free)
//

int test1_12_Mutator::mutatorTesta() 
{
	// Find the entry point to the procedure "test1_12_func2"
	const char *funcName = "test1_12_func2";
	BPatch_Vector<BPatch_function *> found_funcs;

	if ((NULL == appImage->findFunction(funcName, found_funcs)) || !found_funcs.size()) 
	{
		logerror("    Unable to find function %s\n", funcName);
		return -1;
	}

	if (1 < found_funcs.size()) 
	{
		logerror("%s[%d]:  WARNING  : found %d functions named %s.  Using the first.\n", 
				__FILE__, __LINE__, found_funcs.size(), funcName);
	}

	BPatch_Vector<BPatch_point *> *point12_2 = found_funcs[0]->findPoint(BPatch_entry);

	if (!point12_2 || (point12_2->size() < 1)) 
	{
		logerror("Unable to find point %s - entry.\n", funcName);
		return -1;
	}

	varExpr12_1 = appThread->malloc(100);

	if (!varExpr12_1) 
	{
		logerror("Unable to allocate 100 bytes in mutatee\n");
		return -1;
	}

	// Heap stress test - allocate memory until we run out, free it all
	//   and then allocate a small amount of memory.

	setExpectError(66); // We're expecting a heap overflow error
	BPatch_variableExpr* memStuff[30000];
	BPatch_variableExpr *temp;
	int count;

	for (count = 0; count < 2000; count++) 
	{
		temp = appThread->malloc(HEAP_TEST_UNIT_SIZE);

		if (!temp) 
		{
			logerror("*** Inferior malloc stress test failed\n"); 
			exit(-1);
		}

		memStuff[count] = temp;
	}

	setExpectError(DYNINST_NO_ERROR);

	int freeCount = 0;

	for (int i =0; i < count; i++) 
	{
		appThread->free(*memStuff[i]);
		freeCount++;
	}

	temp = appThread->malloc(500); 

	if (!temp) 
	{
		logerror("*** Unable to allocate memory after using then freeing heap\n");
	}

	BPatch_Vector<BPatch_function *> bpfv;
	char *fn = "test1_12_call1";

	if (NULL == appImage->findFunction(fn, bpfv) || !bpfv.size()
			|| NULL == bpfv[0])
	{
		logerror("    Unable to find function %s\n", fn);
		return -1;
	}

	BPatch_function *call12_1_func = bpfv[0];

	BPatch_Vector<BPatch_snippet *> nullArgs;
	BPatch_funcCallExpr call12_1Expr(*call12_1_func, nullArgs);

	checkCost(call12_1Expr);
	snippetHandle12_1 = appThread->insertSnippet(call12_1Expr, *point12_2);

	if (!snippetHandle12_1) 
	{
		logerror("Unable to insert snippet to call function \"%s.\"\n",
				fn);
		return -1;
	}

	return 0;
}

int test1_12_Mutator::mutatorTestb() 
{
	waitUntilStopped(BPatch::bpatch, appThread, 12, "insert/remove and malloc/free");

	// remove instrumentation and free memory
	if (!appThread->deleteSnippet(snippetHandle12_1)) 
	{
		logerror("**Failed test #12 (insert/remove and malloc/free)\n");
		logerror("    deleteSnippet returned an error\n");
		return -1;
	}

	appThread->free(*varExpr12_1);

	// Try removing NULL as a snippet
	if (appThread->deleteSnippet(NULL)) 
	{
		logerror("**Failed test #12 (insert/remove and malloc/free)\n");
		logerror("    deleteSnippet returned success when deleting NULL\n");
		return -1;
	}

	// continue process
	appThread->continueExecution();

	return 0;
}

test_results_t test1_12_Mutator::executeTest() 
{
	test_results_t retval;

	int result = mutatorTesta();

	if (result != 0) 
	{
		return FAILED;
	}

	appThread->continueExecution();
	result = mutatorTestb();

	if (result != 0) 
	{
		retval = FAILED;
	} 
	else 
	{
		retval = PASSED;
	}

	return retval;
}
