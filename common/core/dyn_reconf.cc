#include "dyn_reconf.h"
#include "log.h"
#include "cache_cntlr.h"


Dyn_reconf::Dyn_reconf(CacheCntlr* cc)
{
	m_last_level=cc;
	p_instruction_count=0;
	p_base_count=0;
	p_last_base_count=0;
	p_base_addr=0;
	p_last_base_addr=0;
	p_mask=0x0003ffc0;
	state=STABLE;
	thresh_diff=1024;
	thresh_count=128;
	tune_count=0;
	phase_change=false;
	PRAK_LOG("INITIALIZING DYNAMIC RECONF");
//	m_last_tx=0;
}

Dyn_reconf::~Dyn_reconf()
{
}

void
Dyn_reconf:: processAddress(core_id_t core_id)
{
	if(state==STABLE)
	{
		p_base_count+=1;

		if(p_base_count >=10000)
		{	p_base_count=0;

			PRAK_LOG("CALLING PHASE TUNING");
			m_last_level->getSlabCntlr()->startTuning(core_id);

			state=PHASE_TUNING;
		}
		
	}
	else if(state==PHASE_TUNING)
	{
		tune_count+=1;
		
		if(tune_count >= 4000)
		{	tune_count=0;
			state=PHASE_CHANGE;
		}
	}
	else if(state==PHASE_CHANGE)
	{
		PRAK_LOG("PHASE CHNAGE OCCURED");
		m_last_level->getSlabCntlr()->reconfigure(core_id);
		state=STABLE;
	}
}

/*
void Dyn_reconf:: processAddress(IntPtr address,core_id_t core_id)
{
//	p_instruction_count=p_instruction_count;

		bool phase_change=false;

		if(p_base_addr!=0)
		{
			p_diff=(p_base_addr)-(address);

			p_diff=(p_diff>0)?p_diff:-p_diff;
//			PRAK_LOG("b:%x c-a %x diff:%d \n",p_base_addr,address,p_diff);	
	
		if(state==STABLE)
		{		
			// new base address
			if(p_diff > thresh_diff)
			{
				// remember current stats
				p_last_base_addr=p_base_addr;
				p_last_base_count=p_base_count;

				//change of state,base_address and count
				state=FIRST_CHANGE;
				p_base_addr=address;				
				p_base_count=0;
			}
			else
			{
				p_base_count+=1;
			}
			
		}
		else if(state==FIRST_CHANGE)
		{
			if(p_diff > thresh_diff)
			{	//address is changed again ,we already saved previous base address
				// this new base address shouldn't be remebered

				//change of state,base_address and count
				state=RE_CHANGE;
				p_base_addr=address;				
				p_base_count=0;
			}
			else
			{
				p_base_count+=1;
				if(p_base_count > thresh_count)
				{
		
					PRAK_LOG("*---------CALLING TUNE------------------*");
					m_last_level->getSlabCntlr()->startTuning(core_id);
					state=PHASE_TUNING;
//					state=STABLE;
//					phase_change=true;
				}
			}
		}	
		else if(state==RE_CHANGE)	
		{
			if(p_diff > thresh_diff)
			{	//address is changing  again and again stay here 

				int new_diff= (p_last_base_addr) - (address);
				new_diff=(new_diff>0)?new_diff:-new_diff;
				if(new_diff < thresh_diff)
				{
					p_base_addr=p_last_base_addr;	
					p_base_count=p_last_base_count;	
					state=STABLE;		
				//	PRAK_LOG("reverting to prev base");
				}
				else
				{

					p_base_addr=address;				
					p_base_count=0;
				}
				//change of base_address and count

			}
			else
			{
				p_base_count+=1;
				if(p_base_count > thresh_count)
				{
					state=PHASE_TUNING;
					PRAK_LOG("*---------CALLING TUNE------------------*");
					m_last_level->getSlabCntlr()->startTuning(core_id);
//					state=STABLE;
//					phase_change=true;
				}
			}	
			
		}
		else if(state==PHASE_TUNING)
		{
			tune_count+=1;
			if(tune_count>=1024)
			{		tune_count=0;
					state=STABLE;
					phase_change=true;
					PRAK_LOG("*--------------Change of phase new b:%x------------------*",p_base_addr);
			}

		}
	}
	else
	{
		p_base_addr=address;
	}	
	//---------------------CALL FOR RECONFIGURATION IF PHASE CHANGE
	if(phase_change)
	{
		m_last_level->getSlabCntlr()->reconfigure(core_id);
	}
} 
*/

void Dyn_reconf:: incrementCount(core_id_t core_id)
{
	p_instruction_count+=1;
//	VERI_LOG("RE1");
	processAddress(core_id);
//	VERI_LOG("RE2");
}


UInt64 Dyn_reconf:: getInstructionCount()
{
	return p_instruction_count;
}

Dyn_reconf::Status Dyn_reconf:: getState()
{
	return state;
}

