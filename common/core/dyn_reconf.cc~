#include "dyn_reconf.h"


Dyn_reconf::Dyn_reconf()
{
	p_instruction_count=0;
	p_base_count=0;
	p_last_base_count=0;
	p_base_addr=0;
	p_last_base_addr=0;
	state=STABLE;
}

Dyn_reconf::~Dyn_reconf()
{
}

void Dyn_reconf:: processAddress(IntPtr address)
{
	p_instruction_count=p_instruction_count;
} 


void Dyn_reconf:: incrementCount()
{
	p_instruction_count+=1;
}


UInt64 Dyn_reconf:: getInstructionCount()
{
	return p_instruction_count;
}

Dyn_reconf::Status Dyn_reconf:: getState()
{
	return state;
}

