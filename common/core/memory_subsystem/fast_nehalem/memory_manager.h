#ifndef __FAST_NEHALEM_H
#define __FAST_NEHALEM_H

#include "memory_manager_fast.h"

namespace FastNehalem
{
   class CacheBase
   {
      public:
         virtual ~CacheBase() {}
         virtual SubsecondTime access(Core::mem_op_t mem_op_type, IntPtr tag) = 0;
   };

   class MemoryManager : public MemoryManagerFast
   {
      private:
         CacheBase *icache, *dcache, *l2cache;
         static CacheBase *l3cache, *dram;

      public:
         MemoryManager(Core* core, Network* network, ShmemPerfModel* shmem_perf_model);
         ~MemoryManager();
		//virtual Dyn_reconf * getReconfigurator(core_id_t m_core_id){return NULL;}
         SubsecondTime coreInitiateMemoryAccessFast(core_id_t m_core_id,
               bool use_icache,
               Core::mem_op_t mem_op_type,
               IntPtr address)
         {
            IntPtr tag = address >> CACHE_LINE_BITS;
            return (use_icache ? icache : dcache)->access(mem_op_type, tag);
         }
	virtual	CacheCntlr* getCacheCntlrAt(core_id_t core_id, MemComponent::component_t mem_component)
	{return NULL;}
   };
}

#endif // __FAST_NEHALEM_H
