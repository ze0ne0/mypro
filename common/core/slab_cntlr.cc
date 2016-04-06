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
	m_num_cores=2;
	
	PRAK_LOG("Initializing slab controller");


	slab_slot	= new Cache**	[m_num_cores];
	isSlabOn 	= new bool**	[m_num_cores];
	access		= new UInt32**	[m_num_cores];
	slot_access 	= new UInt32*	[m_num_cores];

	for(UInt32 k=0;k< m_num_cores;k++)
	{
			

		slab_slot[k]	= new Cache*	[m_num_slots];
		isSlabOn[k]	= new bool*	[m_num_slots];
		access[k]   	= new UInt32*	[m_num_slots];
		slot_access[k] 	= new UInt32	[m_num_slots];

		for(UInt32 i=0;i<m_num_slots;i++)
		{
			slab_slot[k][i]=new Cache(name,cfg_name,0,m_num_sets_per_slab,m_slab_assoc,64,r_policy,
					    CacheBase::SHARED_CACHE,
					    CacheBase::parseAddressHash(hash_function),fault_injector);	

			slot_access[k][i]=0;

			isSlabOn[k][i]=new bool [m_num_slabs_per_slot];

			access[k][i]=new UInt32 [m_num_slabs_per_slot];


			for(UInt32 j=0;j<m_num_slabs_per_slot;j++)
			{
				isSlabOn[k][i][j]= j==0;
				access[k][i][j]=0;
			}
			//PRAK_LOG("ISSLABoN0 0:%d 1:%d 2:%d 3:%d ",isSlabOn_0[i][0],isSlabOn_0[i][1],isSlabOn_0[i][2],isSlabOn_0[i][3]);
			//PRAK_LOG("ISSLABoN1 0:%d 1:%d 2:%d 3:%d ",isSlabOn_1[i][0],isSlabOn_1[i][1],isSlabOn_1[i][2],isSlabOn_1[i][3]);
		}
	}
}

void
SlabCntlr:: reconfigure(core_id_t core_id)
{			
	for(UInt32 i=0;i< m_num_slots;i++)
	{
		for(UInt32 j=0;j< m_num_slabs_per_slot;j++)
		{
			if(access[core_id][i][j] > 128 && isSlabOn[core_id][i][j]==false)
			{
				PRAK_LOG("TURN ON core:%d slot:%d slab :%d",core_id,i,j);
				PRAK_LOG("DO BLOCK TRANSFER");
			}
		}	
	}
}


SlabCntlr::~SlabCntlr()
{

}


