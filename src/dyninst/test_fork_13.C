/*
 * See the dyninst/COPYRIGHT file for copyright information.
 * 
 * We provide the Paradyn Tools (below described as "Paradyn")
 * on an AS IS basis, and do not warrant its validity or performance.
 * We reserve the right to update, modify, or discontinue this
 * software at any time.  We shall have no obligation to supply such
 * updates or modifications or any other form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

// $Id: test_fork_13.C,v 1.1 2008/10/30 19:21:34 legendre Exp $
/*
 * #Name: test7_9
 * #Desc: Memory deallocate in parent
 * #Dep: 
 */

#include "BPatch.h"
#include "BPatch_Vector.h"
#include "BPatch_thread.h"
#include "BPatch_snippet.h"
#include "BPatch_point.h"

#include "test_lib.h"
#include "test_lib_test7.h"

#include "dyninst_comp.h"
class test_fork_13_Mutator : public DyninstMutator {
private:
  BPatch *bpatch;

public:
  virtual bool hasCustomExecutionPath() { return true; }
  virtual test_results_t setup(ParameterDict &param);
  virtual test_results_t executeTest();
};
extern "C" DLLEXPORT TestMutator *test_fork_13_factory() {
  return new test_fork_13_Mutator();
}

static bool parentDone;
static bool childDone;
static bool passedTest;
static BPatch_thread *parentThread;
static BPatch_thread *childThread;
static int msgid;


// FIXME This comment is inconsistent.  Figure out what this test is really
// doing and clear it up.
/* Verify that if a variable in the parent process is freed with 
   BPatch_thread::free, the corresponding variable in the child process
   isn't also deleted.

   parent/child: malloc a variable
   parent/child: oneTimeCode(snippetA)  (malloced variable = 10)
   --- fork ---
   child:  insert snippet B  (malloced_var += 5);
   parent: free(getInheritedVariable(malloced_var))

   --- run  ---
   parent: verify malloced_var = 15
   (no way to verify the child variable has indeed been freed)
*/

static BPatch_variableExpr *var7_9p;
static BPatch_variableExpr *var7_9c;

static void prepareTestCase9(procType proc_type, BPatch_thread *thread, forkWhen when)
{

   if(proc_type == Parent_p  &&  when == PreFork) {
       BPatch_image *parImage = thread->getProcess()->getImage();
       var7_9p = thread->getProcess()->malloc(*(parImage->findType("int")));
      if(doError(&passedTest, (var7_9p==NULL),
		 "  Unable to malloc variable in parent\n")) return;

      BPatch_arithExpr a_expr7_9p(BPatch_assign, *var7_9p,
				  BPatch_constExpr(10));
      thread->oneTimeCode(a_expr7_9p);
   } else if(proc_type == Parent_p  &&  when == PostFork) {
      // can't delete var7_9p here, since then the getInheritedVariable
      // would be operating on a freed variable
   } else if(proc_type == Child_p  &&  when == PostFork) {
       var7_9c = thread->getProcess()->getInheritedVariable(*var7_9p);
       parentThread->getProcess()->free(*var7_9p);

       BPatch_image *childImage = thread->getProcess()->getImage();

      BPatch_Vector<BPatch_function *> found_funcs;
      const char *inFunction = "test_fork_13_func1";
      if ((NULL == childImage->findFunction(inFunction, found_funcs, 1)) || !found_funcs.size()) {
	logerror("    Unable to find function %s\n",
		inFunction);
	exit(1);
      }
      
      if (1 < found_funcs.size()) {
	logerror("%s[%d]:  WARNING  : found %d functions named %s.  Using the first.\n", 
		__FILE__, __LINE__, found_funcs.size(), inFunction);
      }
      
      BPatch_Vector<BPatch_point *> *points7_9c = found_funcs[0]->findPoint(BPatch_entry);

      if(doError(&passedTest, !points7_9c || ((*points7_9c).size() == 0),
		 "  Unable to find entry point to \"test_fork_13_func1\".\n")) return;
      BPatch_point *point7_9c = (*points7_9c)[0];

      BPatch_arithExpr a_expr7_9c(BPatch_plus, *var7_9c, BPatch_constExpr(5));
      BPatch_arithExpr b_expr7_9c(BPatch_assign, *var7_9c, a_expr7_9c);

      thread->getProcess()->insertSnippet(b_expr7_9c, *point7_9c, BPatch_callBefore);
   }
}

static void checkTestCase9(procType proc_type, BPatch_thread */*thread*/) {
   char varname[50];
   sprintf(varname,"test%d malloced var",9);

   if(proc_type == Child_p) {
      if(! verifyProcMemory(varname, var7_9c, 15, proc_type)) {
	 passedTest = false;
      }
   }
}

/* We make changes at post-fork */
static void postForkFunc(BPatch_thread *parent, BPatch_thread *child)
{
    //dprintf("in postForkFunc\n");
    /* For later identification */
    childThread = child;
    dprintf("Preparing tests on parent\n");
    prepareTestCase9(Parent_p, parent, PostFork);
    dprintf("Preparing tests on child\n");
    prepareTestCase9(Child_p,  child,  PostFork);
    dprintf("Fork handler finished (parent %p, child %p)\n", parent, child);
}

/* And verify them when they exit */
static void exitFunc(BPatch_thread *thread, BPatch_exitType exit_type) {
    dprintf("Exit func called\n");
    if (thread == parentThread) {
        dprintf("Parent exit reached, checking...\n");
        checkTestCase9(Parent_p, thread);
        parentDone = true;
        dprintf("Parent done\n");
    }
    else if (thread == childThread) {
        dprintf("Child exit reached, checking...\n");
        checkTestCase9(Child_p, thread);
        dprintf("Child done\n");
        childDone = true;
    }
    else {
        dprintf("Thread ptr 0x%x, parent 0x%x, child 0x%x\n",
                thread, parentThread, childThread);
        assert(0 && "Unexpected BPatch_thread in exitFunc");
    }
    return;
}

static void initialPreparation(BPatch_thread *parent)
{
   //cerr << "in initialPreparation\n";
    assert(parent->getProcess()->isStopped());

   //cerr << "ok, inserting instr\n";
   prepareTestCase9(Parent_p, parent, PreFork);
}

static int mutatorTest(BPatch *bpatch, BPatch_thread *appThread)
{
    if ( !setupMessaging(&msgid) )
    {
       passedTest = false;
       return passedTest;
    }

    parentThread = appThread;

    initialPreparation(parentThread);
    /* ok, do the fork */;
                         parentThread->getProcess()->continueExecution();

    /* the rest of the execution occurs in postForkFunc() */
    /* Secondary test: we should not have to manually continue
       either parent or child at any point */

    while ( !parentThread->getProcess()->isTerminated() )
    {
       bpatch->waitForStatusChange();
    }

    // At this point if childThread == NULL the postfork handler failed
    // to run.  Fail gracefully instead of segfaulting on 
    // childThread->isTerminated()
    if (doError(&passedTest, childThread == NULL,
             "childThread == NULL: postForkFunc must not have run\n") )
    {
       return passedTest;
    }
    
    while ( !childThread->getProcess()->isTerminated() )
    {
       bpatch->waitForStatusChange();
    }
    
    return passedTest;
}

test_results_t test_fork_13_Mutator::executeTest() {
  // Initialize global variables
  parentDone = false;
  childDone = false;
  passedTest = true;
  parentThread = NULL;
  childThread = NULL;
  msgid = -1;

  // Register callbacks
  bpatch->registerPostForkCallback(postForkFunc);
  bpatch->registerExitCallback(exitFunc);

  bool passed = mutatorTest(bpatch, appThread);

  // Remove callbacks upon test completion
  bpatch->registerPostForkCallback(NULL);
  bpatch->registerExitCallback(NULL);

  showFinalResults(passed, 9);
  if ( passed )
    return PASSED;
  else
    return FAILED;
}

test_results_t test_fork_13_Mutator::setup(ParameterDict &param) {
#ifdef os_windows_test
  return SKIPPED;
#else
  bpatch = (BPatch *)(param["bpatch"]->getPtr());
  appThread = (BPatch_thread *)(param["appThread"]->getPtr());

  return PASSED;
#endif
}
