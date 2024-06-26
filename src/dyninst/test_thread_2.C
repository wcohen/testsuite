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

// $Id: test_thread_2.C,v 1.1 2008/10/30 19:22:28 legendre Exp $
/*
 * #Name: test12_3
 * #Desc: thread create callback
 * #Dep: 
 * #Notes:
 */
#include <vector>
using std::vector;
#include "BPatch.h"
#include "BPatch_Vector.h"
#include "BPatch_thread.h"
#include "BPatch_snippet.h"

#include "test_lib.h"
#include "test12.h"

#include "dyninst_comp.h"

class test_thread_2_Mutator : public DyninstMutator {
private:
  BPatch *bpatch;

  void dumpVars();
  bool setVar(const char *vname, void *addr, int testno, const char *testname);
  bool getVar(const char *vname, void *addr, int len, int testno,
	      const char *testname);

public:
  virtual bool hasCustomExecutionPath() { return true; }
  virtual test_results_t setup(ParameterDict &param);
  virtual test_results_t executeTest();
};
extern "C" DLLEXPORT TestMutator *test_thread_2_factory() {
  return new test_thread_2_Mutator();
}

#define TESTNO 2
#define TESTNAME "test_thread_2"
#define TESTDESC "thread create callback"

void test_thread_2_Mutator::dumpVars() {
  BPatch_Vector<BPatch_variableExpr *> vars;
  appImage->getVariables(vars);
  for (unsigned int i = 0; i < vars.size(); ++i) {
    logerror("\t%s\n", vars[i]->getName());
  }
}


// Returns true on error, false on success
bool test_thread_2_Mutator::setVar(const char *vname, void *addr, int testno,
				   const char *testname) {
  BPatch_variableExpr *v;
   void *buf = addr;
   if (NULL == (v = appImage->findVariable(vname))) {
      logerror("**Failed test #%d (%s)\n", testno, testname);
      logerror("  cannot find variable %s, avail vars:\n", vname);
      dumpVars();
      return true;
   }

   if (! v->writeValue(buf, sizeof(int),true)) {
      logerror("**Failed test #%d (%s)\n", testno, testname);
      logerror("  failed to write call site var to mutatee\n");
      return true;
   }
   return false; // No error
}


// Returns true on error, false on success
bool test_thread_2_Mutator::getVar(const char *vname, void *addr, int len,
				   int testno, const char *testname) {
   BPatch_variableExpr *v;
   if (NULL == (v = appImage->findVariable(vname))) {
      logerror("**Failed test #%d (%s)\n", testno, testname);
      logerror("  cannot find variable %s: avail vars:\n", vname);
      dumpVars();
      return true;
   }

   if (! v->readValue(addr, len)) {
      logerror("**Failed test #%d (%s)\n", testno, testname);
      logerror("  failed to read var in mutatee\n");
      return true;
   }
   return false; // No error
}


static vector<unsigned long> callback_tids;
static int test3_threadCreateCounter = 0;
static void threadCreateCB(BPatch_process * proc, BPatch_thread *thr)
{
  assert(thr);  
  if (debugPrint())
     dprintf("%s[%d]:  thread %lu start event for pid %d\n", __FILE__, __LINE__,
             thr->getTid(), proc->getPid());
  test3_threadCreateCounter++;
  callback_tids.push_back(thr->getTid());
  if (thr->isDeadOnArrival()) {
     dprintf("%s[%d]:  thread %lu is doa \n", __FILE__, __LINE__, thr->getTid());
  }
}

// static bool mutatorTest3and4(int testno, const char *testname)
test_results_t test_thread_2_Mutator::executeTest() {
  BPatch_process *appProc = appThread->getProcess();
  if (appProc && !appProc->supportsUserThreadEvents()) {
    logerror("System does not support user thread events\n");
    appThread->getProcess()->terminateExecution();
    return SKIPPED;
  }
  test3_threadCreateCounter = 0;
  callback_tids.clear();

  BPatchAsyncThreadEventCallback createcb = threadCreateCB;
  if (!bpatch->registerThreadEventCallback(BPatch_threadCreateEvent, createcb))
  {
    FAIL_MES(TESTNAME, TESTDESC);
    logerror("%s[%d]:  failed to register thread callback\n",
	     __FILE__, __LINE__);
    appThread->getProcess()->terminateExecution();
    return FAILED;
  }

#if 0
  //  unset mutateeIde to trigger thread (10) spawn.
  int zero = 0;
  // FIXME Check the return code for setVar
  setVar("mutateeIdle", (void *) &zero, TESTNO, TESTDESC);
  dprintf("%s[%d]:  continue execution for test %d\n", __FILE__, __LINE__, TESTNO);
  appThread->continueExecution();
#endif

  if( !appProc->continueExecution() ) {
      logerror("%s[%d]: failed to continue process\n", FILE__, __LINE__);
      appProc->terminateExecution();
      return FAILED;
  }

  //  wait until we have received the desired number of events

  int err = 0;
  BPatch_Vector<BPatch_thread *> threads;
  assert(appProc);
  appProc->getThreads(threads);
  int active_threads = 11; // FIXME Magic number
  threads.clear();
  while (((test3_threadCreateCounter < TEST3_THREADS)
         || (active_threads > 1)) && !appProc->isTerminated() ) {
    dprintf("%s[%d]: waiting for completion for test; ((%d < %d) || (%d > 1)) && !(%d)\n",
	    __FILE__, __LINE__, test3_threadCreateCounter, TEST3_THREADS,
	    active_threads, 1, appProc->isTerminated());
    if( !bpatch->waitForStatusChange() ) {
        logerror("%s[%d]: failed to wait for events\n", __FILE__, __LINE__);
        err = 1;
        break;
    }

    appProc->getThreads(threads);
    active_threads = threads.size();
    threads.clear();
  }

  if( appProc->isTerminated() ) {
      logerror("%s[%d]:  BAD NEWS:  somehow the process died\n", __FILE__, __LINE__);
      return FAILED;
  }

  dprintf("%s[%d]: ending test %d, num active threads = %d\n",
            __FILE__, __LINE__, TESTNO, active_threads);
  dprintf("%s[%d]:  stop execution for test %d\n", __FILE__, __LINE__, TESTNO);
  appProc->stopExecution();

  //   read all tids from the mutatee and verify that we got them all
  unsigned long mutatee_tids[TEST3_THREADS];
  const char *threads_varname = "test3_threads";
  if (getVar(threads_varname, (void *) mutatee_tids,
	     (sizeof(unsigned long) * TEST3_THREADS),
	     TESTNO, TESTDESC)) {
    appProc->terminateExecution();
    return FAILED;
  }

  if (debugPrint()) {
    dprintf("%s[%d]:  read following tids for test%d from mutatee\n", __FILE__, __LINE__, TESTNO);

    for (unsigned int i = 0; i < TEST3_THREADS; ++i) {
       dprintf("\t%lu\n", mutatee_tids[i]);
    }
  }

  for (unsigned int i = 0; i < TEST3_THREADS; ++i) {
     bool found = false;
     for (unsigned int j = 0; j < callback_tids.size(); ++j) {
       if (callback_tids[j] == mutatee_tids[i]) {
         found = true;
         break;
       }
     }

    if (!found) {
      FAIL_MES(TESTNAME, TESTDESC);
      logerror("%s[%d]:  could not find record for tid %lu: have these:\n",
             __FILE__, __LINE__, mutatee_tids[i]);
       for (unsigned int j = 0; j < callback_tids.size(); ++j) {
          logerror("%lu\n", callback_tids[j]);
       }
      err = 1;
      break;
    }
  }

  dprintf("%s[%d]: removing thread callback\n", __FILE__, __LINE__);
  if (!bpatch->removeThreadEventCallback(BPatch_threadCreateEvent, createcb)) {
    FAIL_MES(TESTNAME, TESTDESC);
    logerror("%s[%d]:  failed to remove thread callback\n",
           __FILE__, __LINE__);
    err = 1;
  }

  if (!err)  {
    logerror("No error reported, terminating process and returning success\n");
    PASS_MES(TESTNAME, TESTDESC);
    appProc->terminateExecution();
    logerror("\t Process terminated\n");
    return PASSED;
  }
  appProc->terminateExecution();
  return FAILED;
}
// extern "C" int test12_3_mutatorMAIN(ParameterDict &param)
test_results_t test_thread_2_Mutator::setup(ParameterDict &param) {
  DyninstMutator::setup(param);
  bpatch = (BPatch *)(param["bpatch"]->getPtr());

  return PASSED;
}
