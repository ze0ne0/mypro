#ifndef __SLAB_CNTLR_H
#define __SLAB_CNTLR_H



#include "fixed_types.h"
#include "cache.h"
#include "cache_base.h"
#include "cache_set.h"
#include "cache_block_info.h"
#include "shared_cache_block_info.h"
#include "utils.h"
#include "hash_map_set.h"
#include "cache_perf_model.h"
#include "shmem_perf_model.h"
#include "log.h"
#include "core.h"
#include "fault_injection.h"



class SlabCntlr
{
	
public:
		SlabCntlr(
	    String name,String cfg_name,
            core_id_t core_id,CacheCntlr *cc,
            FaultInjector *fault_injector = NULL,
            AddressHomeLookup *ahl = NULL);

		~SlabCntlr();
private:
		CacheCntlr *cntlr;
		Cache ****slab_slot;

		bool ***isSlabOn;
		bool ****a_pattern;

		UInt32 ***access;

		UInt32 **slot_access;

		

		UInt32 m_num_slots;
		UInt32 m_num_cores;
		UInt32 m_num_slabs_per_slot;
		UInt32 m_num_sets_per_slab;
		UInt32 m_slab_assoc;
	
public:
		void reconfigure(core_id_t core_id);
		Cache **** getSlabSlotPtr(){return slab_slot;}

//------------------------------------------------
		         bool operationPermissibleinCache_slab(core_id_t m_core_id,
               IntPtr address, Core::mem_op_t mem_op_type, CacheBlockInfo **cache_block_info = NULL);

		UInt32 getSlab(const IntPtr addr,UInt32 &slot_index,core_id_t m_core_id) const;


		bool **** getPattern()
		{ return a_pattern;}



         SharedCacheBlockInfo* getCacheBlockInfo_slab(IntPtr address,core_id_t m_core_id);
         CacheState::cstate_t getCacheState_slab(IntPtr address,core_id_t m_core_id);
         CacheState::cstate_t getCacheState_slab(CacheBlockInfo *cache_block_info);
         SharedCacheBlockInfo* setCacheState_slab(IntPtr address,CacheState::cstate_t cstate,core_id_t m_core_id);





//------------------------------------------------

};

#endif
