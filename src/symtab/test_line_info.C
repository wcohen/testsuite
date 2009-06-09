/*
 * Copyright (c) 1996-2008 Barton P. Miller
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

#include "symtab_comp.h"
#include "test_lib.h"

#include "Symtab.h"
#include "Symbol.h"

using namespace Dyninst;
using namespace SymtabAPI;

class test_line_info_Mutator : public SymtabMutator {
public:
   test_line_info_Mutator() { };
   virtual test_results_t executeTest();
};

extern "C" DLLEXPORT TestMutator* test_line_info_factory()
{
   return new test_line_info_Mutator();
}

test_results_t test_line_info_Mutator::executeTest()
{
   std::vector<Function *> funcs;
   bool result = symtab->findFunctionsByName(funcs, std::string("lookup_func"));

   if (!result || !funcs.size() )
   {
      logerror("[%s:%u] - Unable to find test_lookup_func\n", 
               __FILE__, __LINE__);
      return FAILED;
   }

   if (funcs.size() != 1)
   {
      logerror("[%s:%u] - Too many functions found??: %d\n", 
               __FILE__, __LINE__, funcs.size());
      return FAILED;
   }


   return PASSED;
}
