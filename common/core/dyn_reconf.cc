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
	state=FIRST_CHANGE;
	thresh_diff=2450;
	thresh_count=256;
//	m_last_tx=0;
}

Dyn_reconf::~Dyn_reconf()
{
}

void Dyn_reconf:: processAddress(IntPtr address,core_id_t core_id)
{
//	p_instruction_count=p_instruction_count;

		bool phase_change=false;

		if(p_base_addr!=0)
		{
			p_diff=(p_base_addr&p_mask)-(address&p_mask);

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

					PRAK_LOG("*--------------Change 1 of phase new b:%x------------------*",p_base_addr);
					state=STABLE;
					phase_change=true;
				}
			}
		}	
		else if(state==RE_CHANGE)	
		{
			if(p_diff > thresh_diff)
			{	//address is changing  again and again stay here 

				int new_diff= (p_last_base_addr&p_mask) - (address&p_mask);
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
					PRAK_LOG("*--------------Change 2 of phase new b:%x------------------*",p_base_addr);
					state=STABLE;
					phase_change=true;
				}
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


void Dyn_reconf:: incrementCount(IntPtr address,core_id_t core_id)
{
	p_instruction_count+=1;
//	VERI_LOG("RE1");
	processAddress(address,core_id);
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

