#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "fixed_types.h"

UInt64 p_instruction_count;
int p_diff;
int thresh_diff;
UInt64 thresh_count;	
UInt64 tune_count;		

UInt64 p_base_count;
UInt64 p_last_base_count;

IntPtr p_base_addr;
IntPtr p_mask;	
IntPtr p_last_base_addr;

enum Status
	      	{
		 STABLE = 0,
		 FIRST_CHANGE,
		 RE_CHANGE,
		 PHASE_TUNING
	      	};
Status state;


void processAddress(IntPtr address,core_id_t core_id)
{
//	p_instruction_count=p_instruction_count;

		bool phase_change=false;

		if(p_base_addr!=0)
		{
			p_diff=(p_base_addr/*&p_mask*/)-(address/*&p_mask*/);

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

					printf("*--------------Change 1 of phase new b:%x------------------*\n",p_base_addr);
					state=PHASE_TUNING;

				}
			}
		}	
		else if(state==RE_CHANGE)	
		{
			if(p_diff > thresh_diff)
			{	//address is changing  again and again stay here 

				int new_diff= (p_last_base_addr/*&p_mask*/) - (address/*&p_mask*/);
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
					printf("*--------------Change 2 of phase new b:%x------------------*\n",p_base_addr);
					state=PHASE_TUNING;
				}
			}	
			
		}else if(state==PHASE_TUNING)
		{
			tune_count=tune_count+1;
			if(tune_count > 512)
			{	printf("tuned \n");
				state=STABLE;
				phase_change=true;
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
		printf("phase changed");
		//m_last_level->getSlabCntlr()->reconfigure(core_id);
	}
} 


int main()
{

	FILE *fp=NULL;
	const int Naccess=32000,thresh=1024;


	IntPtr currAddress;
	p_instruction_count=0;
	p_base_count=0;
	p_last_base_count=0;
	p_base_addr=0;
	p_last_base_addr=0;
	p_mask=0x0003ffc0;
	state=FIRST_CHANGE;
	thresh_diff=1024;
	thresh_count=128;


	//--------------------------------------------------


	fp=fopen("file1","r");
	if(fp == NULL)
	{
		perror("File open error\n");
		return -1;
	}			
	

	for(int i=0;i<Naccess;i++)
	{
		fscanf(fp,"%x",&currAddress);
		processAddress(currAddress,0);
	
	}
	
	fclose(fp);
	return 0;
}
