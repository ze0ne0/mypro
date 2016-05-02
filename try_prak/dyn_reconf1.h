#ifndef DYN_RECONF_H
#define DYN_RECONF_H

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
		void incrementCount()
		{
			//p_instruction_count+=1;
		}
		void processAddress(IntPtr );
		UInt64 getInstructionCount()
		{
			return p_instruction_count;
		}
		Status getState()
		{
			return state;
		}
	private:
		UInt64 p_instruction_count;
		UInt64 p_base_count;
		UInt64 p_last_base_count;
		IntPtr p_base_addr;
		IntPtr p_last_base_addr;
		Status state;				
		
//	protected:
};

#endif
