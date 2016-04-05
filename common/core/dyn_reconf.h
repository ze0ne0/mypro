#ifndef __DYN_RECONF_H
#define __DYN_RECONF_H

#include "fixed_types.h"

class Dyn_reconf
{
	
public:
		enum Status
	      	{
		 STABLE = 0,
		 FIRST_CHANGE,
		 RE_CHANGE
	      	};

		Dyn_reconf();
		~Dyn_reconf();
		void incrementCount(IntPtr address);
		void processAddress(IntPtr);
		UInt64 getInstructionCount();
		Status getState();
	private:
		UInt64 p_instruction_count;
		int p_diff;
		int thresh_diff;

		UInt64 p_base_count;
		UInt64 p_last_base_count;

		IntPtr p_base_addr;
		IntPtr p_last_base_addr;

		Status state;				
	
//	protected:
};

#endif
