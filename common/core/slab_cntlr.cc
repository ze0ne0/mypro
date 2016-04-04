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

	slab_slot_0= new Cache*[m_num_slots];
	slab_slot_1= new Cache*[m_num_slots];

	isSlabOn_0 = new bool*[m_num_slots];
	isSlabOn_1 = new bool*[m_num_slots];

	for(UInt32 i=0;i<m_num_slots;i++)
	{
		slab_slot_0[i]=new Cache(name,cfg_name,0,m_num_sets_per_slab,m_slab_assoc,64,r_policy,
			            CacheBase::SHARED_CACHE,
			            CacheBase::parseAddressHash(hash_function),fault_injector);	

		slab_slot_1[i]=new Cache(name,cfg_name,1,m_num_sets_per_slab,m_slab_assoc,64,r_policy,
			            CacheBase::SHARED_CACHE,
			            CacheBase::parseAddressHash(hash_function),fault_injector);	

		isSlabOn_0[i]=new bool [m_num_slabs_per_slot];
		isSlabOn_1[i]=new bool [m_num_slabs_per_slot];

		for(UInt32 j=0;j<m_num_slabs_per_slot;j++)
		{
			isSlabOn_0[i][j]= j==0;
			isSlabOn_1[i][j]= j==0;
		}
		PRAK_LOG("ISSLABoN0 0:%d 1:%d 2:%d 3:%d ",isSlabOn_0[i][0],isSlabOn_0[i][1],isSlabOn_0[i][2],isSlabOn_0[i][3]);
		PRAK_LOG("ISSLABoN1 0:%d 1:%d 2:%d 3:%d ",isSlabOn_1[i][0],isSlabOn_1[i][1],isSlabOn_1[i][2],isSlabOn_1[i][3]);
	}



	

/*Cache *p_cache = new Cache(name,cfg_name,core_id,m_num_sets_per_slab,m_slab_assoc,64,r_policy,
            CacheBase::SHARED_CACHE,
            CacheBase::parseAddressHash(hash_function),fault_injector);	
*/
}

SlabCntlr::~SlabCntlr()
{

}


