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
#include "subsecond_time.h"



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

		UInt64 L2_access,L2_hits,Dram_access;
		UInt64 mem_access,hits,dram_access;

		SubsecondTime t_now,t_prev;
		UInt32 num_reconf;
		UInt32 **slot_access;

		ShmemPerfModel* m_shmem_perf;

		Lock a_lock,b_lock,c_lock,recon;	

		UInt32 m_num_slots;
		UInt32 m_num_cores;
		UInt32 m_num_slabs_per_slot;
		UInt32 m_num_sets_per_slab;
		UInt32 m_slab_assoc;

		UInt32 m_block_transfer,m_last_tx;

		UInt32 m_log_blocksize;
		UInt32 m_log_num_slabs_per_slot;
		UInt32 m_log_num_sets_per_slab;


	
public:
		void reconfigure(core_id_t core_id);
		Cache **** getSlabSlotPtr(){return slab_slot;}

//------------------------------------------------
		bool operationPermissibleinCache_slab(core_id_t m_core_id,
               	IntPtr address, Core::mem_op_t mem_op_type, CacheBlockInfo **cache_block_info = NULL,bool record_stat=false);

		UInt32 getSlab(const IntPtr addr,UInt32 &slot_index,core_id_t m_core_id,UInt32 *slab_orig=NULL) const;

		bool **** getPattern()
		{ 
			return a_pattern;
		}
		void incrementDramAccess()
		{
			Dram_access+=1;
			dram_access+=1;
		}
		int getActiveSlab();
		int getSetCount(UInt32 slot,UInt32 slab);
		void setPerfModel(ShmemPerfModel* shmem_perf)
		{
			m_shmem_perf=shmem_perf;
		}

	UInt32 getSlabAssoc(){return m_slab_assoc;}
	UInt32 getSlab_m_log_blocksize(){return m_log_blocksize;}
	UInt32 getSlab_m_num_sets_per_slab(){return m_num_sets_per_slab;}

         SharedCacheBlockInfo* getCacheBlockInfo_slab(IntPtr address,core_id_t m_core_id,bool record_stat=false);
         CacheState::cstate_t getCacheState_slab(IntPtr address,core_id_t m_core_id);
         CacheState::cstate_t getCacheState_slab(CacheBlockInfo *cache_block_info);
         SharedCacheBlockInfo* setCacheState_slab(IntPtr address,CacheState::cstate_t cstate,core_id_t m_core_id);
	 UInt32 getSetIndex(IntPtr address);
	void print_stats();
	void reset_stats();

//------------------------------------------------
};

#endif
