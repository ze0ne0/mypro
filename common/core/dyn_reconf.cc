#include "dyn_reconf.h"
#include "log.h"


Dyn_reconf::Dyn_reconf()
{
	p_instruction_count=0;
	p_base_count=0;
	p_last_base_count=0;
	p_base_addr=0;
	p_last_base_addr=0;
	state=STABLE;
	thresh_diff=1500;
}

Dyn_reconf::~Dyn_reconf()
{
}

void Dyn_reconf:: processAddress(IntPtr address)
{
//	p_instruction_count=p_instruction_count;

		if(p_base_addr!=0)
		{
			p_diff=p_base_addr-address;
			p_diff=(p_diff>0)?p_diff:-p_diff;
			PRAK_LOG("b:%x c-a %x diff:%d \n",p_base_addr,address,p_diff);	
	
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
			}
		}	
		else if(state==RE_CHANGE)	
		{
			if(p_diff > thresh_diff)
			{	//address is changing  again and again stay here 

				//change of base_address and count
				p_base_addr=address;				
				p_base_count=0;
			}
			else
			{
				p_base_count+=1;
			}	
			if(p_base_count > 128)
			{
				PRAK_LOG("*--------------Change of phase new b:%x------------------*\n",p_base_addr);
				state=STABLE;
			}
		}
	}
	else
	{
		p_base_addr=address;
	}	
} 


void Dyn_reconf:: incrementCount(IntPtr address)
{
	p_instruction_count+=1;
	processAddress(address);
}


UInt64 Dyn_reconf:: getInstructionCount()
{
	return p_instruction_count;
}

Dyn_reconf::Status Dyn_reconf:: getState()
{
	return state;
}

