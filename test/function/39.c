/* 
TEST_HEADER
 id = $Id$
 summary = try to provoke request.dylan.170463 using LO pool
 language = c
 link = testlib.o awlfmt.o
END_HEADER
*/

#include "testlib.h"
#include "mpscawl.h"
#include "mpscamc.h"
#include "mpsavm.h"
#include "mpsclo.h"
#include "awlfmt.h"


#define genCOUNT (3)

static mps_gen_param_s testChain[genCOUNT] = {
  { 6000, 0.90 }, { 8000, 0.65 }, { 16000, 0.50 } };


static void test(void *stack_pointer)
{
 mps_arena_t arena;
 mps_pool_t poolamc, poollo;
 mps_thr_t thread;
 mps_root_t root;

 mps_fmt_t format;
 mps_chain_t chain;
 mps_ap_t apamc, aplo;

 mycell *a[100];

 int i;
 int j;
 int k,z;

 alloccomments = 1;
 formatcomments = 1;

 cdie(mps_arena_create(&arena, mps_arena_class_vm(), mmqaArenaSIZE),
      "create arena");

 die(mps_thread_reg(&thread, arena), "register thread");
 cdie(mps_root_create_thread(&root, arena, thread, stack_pointer), "thread root");
 die(mps_fmt_create_A(&format, arena, &fmtA), "create format");
 cdie(mps_chain_create(&chain, arena, genCOUNT, testChain), "chain_create");

 die(mmqa_pool_create_chain(&poolamc, arena, mps_class_amc(), format, chain),
     "create pool(amc)");

 die(mps_pool_create(&poollo, arena, mps_class_lo(), format),
     "create pool(lo)");

 cdie(
  mps_ap_create(&aplo, poollo, mps_rank_exact()),
  "create ap");

 cdie(
  mps_ap_create(&apamc, poolamc, mps_rank_exact()),
  "create ap");

 for(i=0; i<100; i++) {
  a[i] = allocone(aplo, 6, 1);
 }

 for(i=0; i<10000; i++) {
  j = ranint(100);
  comment("New object %i", j);
  a[j] = allocone(aplo, 5+ranint(50), 1);
  k = ranint(50);
  z = ranint(5);
  comment("setting %i (%p) %i", k, a[k], z);
  setref(a[k], z, a[j]);
  (void)allocdumb(apamc, 0x400*64, 0);
 }

 mps_arena_park(arena);
 mps_ap_destroy(aplo);
 mps_ap_destroy(apamc);
 comment("Destroyed aps.");

 mps_pool_destroy(poolamc);
 mps_pool_destroy(poollo);
 comment("Destroyed pools.");

 mps_chain_destroy(chain);
 mps_fmt_destroy(format);

 mps_root_destroy(root);
 mps_thread_dereg(thread);

 mps_arena_destroy(arena);
 comment("Destroyed arena.");
}


int main(void)
{
 run_test(test);
 pass();
 return 0;
}
