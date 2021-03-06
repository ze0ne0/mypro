#include "slab_cntlr.h"
#include "simulator.h"
#include "cache.h"
#include "log.h"


SlabCntlr::SlabCntlr(
	    String name,String cfg_name,
            core_id_t core_id,CacheCntlr *cc,
            FaultInjector *fault_injector,
            AddressHomeLookup *ahl)
{
	cntlr=cc;


	m_block_transfer=0;
	m_num_slots=64;
	m_num_slabs_per_slot=4;
	m_num_sets_per_slab=16;
	m_slab_assoc=8;
	String r_policy="lru";
	String hash_function="mod";
	m_num_cores=1;
	L2_access=0;L2_hits=0;

//---------------------------------------------------------------------------------
	m_log_blocksize = floorLog2(64);
	m_log_num_slabs_per_slot=floorLog2(m_num_slabs_per_slot);
	m_log_num_sets_per_slab=floorLog2(m_num_sets_per_slab);
//------------------------------------------------------------------------------------
	PRAK_LOG("Initializing slab controller");


	slab_slot	= new Cache***	[m_num_cores];
	isSlabOn 	= new bool**	[m_num_cores];
	a_pattern 	= new bool***	[m_num_cores];
	access		= new UInt32**	[m_num_cores];
	slot_access 	= new UInt32*	[m_num_cores];

	for(UInt32 k=0;k< m_num_cores;k++)
	{
			

		slab_slot[k]	= new Cache**	[m_num_slots];
		isSlabOn[k]	= new bool*	[m_num_slots];
		a_pattern[k] 	= new bool**	[m_num_slots];
		access[k]   	= new UInt32*	[m_num_slots];
		slot_access[k] 	= new UInt32	[m_num_slots];

		for(UInt32 i=0;i<m_num_slots;i++)
		{
			slab_slot[k][i]=new Cache*	[m_num_slabs_per_slot];

			slot_access[k][i]=0;

			isSlabOn[k][i]=new bool [m_num_slabs_per_slot];

			a_pattern[k][i]=new bool* [m_num_slabs_per_slot];

			access[k][i]=new UInt32 [m_num_slabs_per_slot];


			for(UInt32 j=0;j<m_num_slabs_per_slot;j++)
			{

			//	PRAK_LOG("CREATING %s cfg:%s ",name.c_str(),cfg_name.c_str());

				slab_slot[k][i][j]=new Cache(name,cfg_name,0,m_num_sets_per_slab,m_slab_assoc,64,r_policy,
					    CacheBase::SHARED_CACHE,
					    CacheBase::parseAddressHash(hash_function),fault_injector);	

				isSlabOn[k][i][j]= j==0;
				access[k][i][j]=0;
				a_pattern[k][i][j]= new bool [m_num_sets_per_slab];
				for(UInt32 s=0;s<m_num_sets_per_slab;s++)
				{
					a_pattern[k][i][j][s]=false;
				}

			}
			//PRAK_LOG("ISSLABoN0 0:%d 1:%d 2:%d 3:%d ",isSlabOn_0[i][0],isSlabOn_0[i][1],isSlabOn_0[i][2],isSlabOn_0[i][3]);
			//PRAK_LOG("ISSLABoN1 0:%d 1:%d 2:%d 3:%d ",isSlabOn_1[i][0],isSlabOn_1[i][1],isSlabOn_1[i][2],isSlabOn_1[i][3]);
		}
	}

	//print_stats();
}

void
SlabCntlr ::print_stats()
{
	for(UInt32 i=0;i<16;i++)
	{
		PRAK_LOG("SLt_%d:%d SLt_%d:%d SLt_%d:%d SLt_%d:%d",(4*i),slot_access[0][4*i],(4*i+1),slot_access[0][4*i+1],(4*i+2),slot_access[0][4*i+2],(4*i+3),slot_access[0][4*i+3]);
	}

	for(UInt32 i=0;i<64;i++)
	{
		PRAK_LOG("S_%d_%d:%d S_%d_%d:%d  S_%d_%d:%d  S_%d_%d:%d ",i,0,access[0][i][0],i,1,access[0][i][1],i,2,access[0][i][2],i,3,access[0][i][3]);
	}	
}

void
SlabCntlr:: reset_stats()
{
	for(UInt32 i=0;i<m_num_slots;i++)
	{	
		slot_access[0][i]=0;

		for(UInt32 j=0;j<m_num_slabs_per_slot;j++)
		{
			access[0][i][j]=0;

			for(UInt32 s=0;s<m_num_sets_per_slab;s++)
			{
				a_pattern[0][i][j][s]=false;
			}
		}
	}
}

void
SlabCntlr:: reconfigure(core_id_t core_id)
{	
	//print_stats();
	cntlr->getSlabLock().acquire();	
	PRAK_LOG("In reconfiguration");	
	for(UInt32 i=0;i< m_num_slots;i++)
	{	
		for(UInt32 j=1;j< m_num_slabs_per_slot;j++)
		{
			if(access[0][i][j] > 160 && isSlabOn[0][i][j]==false)
			{
				isSlabOn[0][i][j]=true;
				PRAK_LOG("TURN ON core:%d slot:%d slab :%d  NES ST:%d",0,i,j,isSlabOn[0][i][j]);
				VERI_LOG("TURN ON core:%d slot:%d slab :%d  NES ST:%d",0,i,j,isSlabOn[0][i][j]);
				m_block_transfer += cntlr->slab_transfer(0,i,0,j);
				PRAK_LOG("DONE BLOCK TRANSFER");
				VERI_LOG("DONE BLOCK TRANSFER");
			}
			else if(access[0][i][j] < 80  && isSlabOn[0][i][j]==true)
			{
				isSlabOn[0][i][j]=false;
				PRAK_LOG("TURN OFF core:%d slot:%d slab :%d  NES ST:%d",0,i,j,isSlabOn[0][i][j]);
				VERI_LOG("TURN OFF core:%d slot:%d slab :%d  NES ST:%d",0,i,j,isSlabOn[0][i][j]);
				m_block_transfer += cntlr->slab_transfer_off(0,i,j,0);
				PRAK_LOG("DONE BLOCK TRANSFER OFF");
				VERI_LOG("DONE BLOCK TRANSFER OFF");
			}
		}	
	}

	PRAK_LOG("BLK_TRNSFER:%d",m_block_transfer);
	VERI_LOG("BLK_TRNSFER:%d",m_block_transfer);
	m_block_transfer=0;
	reset_stats();
	cntlr->getSlabLock().release();
//print_stats();
}


SlabCntlr::~SlabCntlr()
{
	PRAK_LOG("###In slab destructor###");
	PRAK_LOG("Number of L2-access %d ",L2_access);
	PRAK_LOG("Number of L2-hits %lld ",L2_hits);

/*
	delete [] isSlabOn;
	delete [] a_pattern;

		delete [] access;
		delete [] slot_access;
		delete [] slab_slot;
*/
	
}
//---------------------------------------------------------------------------------




UInt32 
SlabCntlr::getSlab(const IntPtr addr,UInt32 &slot_index,core_id_t m_core_id,UInt32 *slab_orig) const
{
	//-------------WORKING CHECKED WITH SCI-CALCULATOR------------

	UInt32 g_set_index=addr>>m_log_blocksize;//eliminate 6 bit block offset;

	slot_index=g_set_index>>(m_log_num_slabs_per_slot + m_log_num_sets_per_slab);//eliminate 2 bit slab + 4 bit local set_index
	slot_index=slot_index % m_num_slots ; // take least significant 6 bits

	g_set_index=g_set_index>> m_log_num_sets_per_slab;//eliminate 4 bit local index ,now slab index + slot + tag remaining
	g_set_index=g_set_index % m_num_slabs_per_slot; //take least significant two bits;

	if(slab_orig!=NULL)
	{
		*slab_orig=g_set_index;
	}
	if(isSlabOn[0][slot_index][g_set_index])
	{
		VERI_LOG("Getslab-success-addr:%x slab:%d slot:%d",addr,g_set_index,slot_index);
		return g_set_index;
	}
	else
	{
	//	VERI_LOG("Getslab-f-addr:%x slab:%d slot:%d",addr,0,slot_index);
		return 0;
	}
	
}

bool 
SlabCntlr::operationPermissibleinCache_slab(core_id_t m_core_id,
               IntPtr address, Core::mem_op_t mem_op_type, CacheBlockInfo **cache_block_info,bool record_stat)
{
 	CacheBlockInfo *block_info = getCacheBlockInfo_slab(address,0,true);
   // returns NULL if block doesn't exist in cache

   if (cache_block_info != NULL)
	  *cache_block_info = block_info;

   bool cache_hit = false;
   CacheState::cstate_t cstate = getCacheState_slab(block_info);

   switch (mem_op_type)
   {
      case Core::READ:
         cache_hit = CacheState(cstate).readable();
         break;

      case Core::READ_EX:
      case Core::WRITE:
         cache_hit = CacheState(cstate).writable();
         break;

      default:
         LOG_PRINT_ERROR("Unsupported mem_op_type: %u", mem_op_type);
         break;
   }

//   MYLOG("address %lx state %c: permissible %d", address, CStateString(cstate), cache_hit);
	if(record_stat)
	{	
		//a_lock.acquire();
		L2_access+=1;			
		//a_lock.release();

		if(cache_hit)
		{
		//	c_lock.acquire();
				L2_hits+=1;
		//	c_lock.release();
		}
	}
   return cache_hit;		
}


SharedCacheBlockInfo*
SlabCntlr::getCacheBlockInfo_slab(IntPtr address,core_id_t m_core_id,bool record_stat)
{
	UInt32 slab_index,slot_index,slab_orig;

	

	if(record_stat)
	{
		slab_index=getSlab(address,slot_index,0,&slab_orig);
		UInt32 set_index=getSetIndex(address);

		//b_lock.acquire();
			slot_access[0][slot_index]+=1;
			access[0][slot_index][slab_orig]+=1;
			a_pattern[0][slot_index][slab_orig][set_index]=true;
		//b_lock.release();
	}
	else
	{
		slab_index=getSlab(address,slot_index,0);
	}	
   return (SharedCacheBlockInfo*) slab_slot[0][slot_index][slab_index]->peekSingleLine(address);
}

UInt32 
SlabCntlr:: getSetIndex(IntPtr addr)
{

	UInt32 set_index=addr>>m_log_blocksize;//eliminate 6 bit block offset;
	set_index=set_index % m_num_sets_per_slab;
	return set_index;
}



CacheState::cstate_t
SlabCntlr::getCacheState_slab(IntPtr address,core_id_t m_core_id)
{
   SharedCacheBlockInfo* cache_block_info = getCacheBlockInfo_slab(address,0);
   return getCacheState_slab(cache_block_info);
}

CacheState::cstate_t
SlabCntlr::getCacheState_slab(CacheBlockInfo *cache_block_info)
{
   return (cache_block_info == NULL) ? CacheState::INVALID : cache_block_info->getCState();
}

SharedCacheBlockInfo*
SlabCntlr::setCacheState_slab(IntPtr address,CacheState::cstate_t cstate,core_id_t m_core_id)
{
   SharedCacheBlockInfo* cache_block_info = getCacheBlockInfo_slab(address,0);
   cache_block_info->setCState(cstate);	
   return cache_block_info;
}
//------------------------------------------------------------------------------------



