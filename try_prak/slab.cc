#include "fixed_types.h"
#include <cstdio>



UInt32 getSlab(IntPtr addr) 
{

	UInt32 slot_index;

	UInt32 g_set_index=addr>>6;//eliminate 6 bit block offset;

	printf("g_set_index eli-6-lsb : %x \n",g_set_index);

	slot_index=g_set_index>>6;//eliminate 2 bit slab + 4 bit local set_index

	printf("slot_index eli-12-lsb : %x \n",slot_index);

	slot_index=slot_index % 64 ; // take least significant 6 bits
	printf("slot_index onlu-6-lsb : %d \n",slot_index);

	g_set_index=g_set_index>>4;//eliminate 4 bit local index ,now slab index + slot + tag remaining
	printf("g_set_index eli-10-lsb : %x \n",g_set_index);

	g_set_index=g_set_index % 4; //take least significant two bits;

	printf("g_set_index only-2-lsb : %d \n",g_set_index);

	return g_set_index;
	
}



int main()
{
	IntPtr addr=0xcd123ab4;
	printf("addr : %x \n",addr);

	UInt32 slab_index=getSlab(addr);
	return 0;
}


