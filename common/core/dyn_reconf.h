#ifndef __DYN_RECONF_H
#define __DYN_RECONF_H

#include "fixed_types.h"
class CacheCntlr;

class Dyn_reconf
{
	
public:
		enum Status
	      	{
		 STABLE = 0,
		 FIRST_CHANGE,
		 RE_CHANGE
	      	};

		Dyn_reconf(CacheCntlr* cc);
		~Dyn_reconf();
		void incrementCount(IntPtr address,core_id_t core_id);
		void processAddress(IntPtr address,core_id_t core_id);
		UInt64 getInstructionCount();
		Status getState();
	private:

		CacheCntlr* m_last_level;

		UInt64 p_instruction_count;
		int p_diff;
		int thresh_diff;
		UInt64 thresh_count;		

		UInt64 p_base_count;
		UInt64 p_last_base_count;

		IntPtr p_base_addr;
		IntPtr p_last_base_addr;

		Status state;				
	
//	protected:
};

#endif
