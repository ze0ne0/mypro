#include "slab_cntlr.h"
#include "simulator.h"
#include "cache.h"
#include "log.h"


SlabCntlr::SlabCntlr(
	    String name,String cfg_name,
            core_id_t core_id,
            FaultInjector *fault_injector,
            AddressHomeLookup *ahl)
{
	m_num_slots=64;
	m_num_slabs_per_slot=4;
	m_num_sets_per_slab=16;
	m_slab_assoc=4;
	String r_policy="lru";
	String hash_function="mod";
	
	PRAK_LOG("Initializing slab controller");

Cache *p_cache = new Cache(name,cfg_name,core_id,m_num_sets_per_slab,m_slab_assoc,64,r_policy,
            CacheBase::SHARED_CACHE,
            CacheBase::parseAddressHash(hash_function),fault_injector);	
}

SlabCntlr::~SlabCntlr()
{

}


