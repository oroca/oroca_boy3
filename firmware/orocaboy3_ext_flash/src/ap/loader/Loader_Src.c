/* Includes ------------------------------------------------------------------*/
#include "hw.h"





/*******************************************************************************
 Description :																			         
 Write data to the device	 														       
 Inputs :																					           
 				Address 	: Write location  										     
 				Size 		: Length in bytes 										     
 				buffer 		: Address where to get the data to write	 
 outputs :																				           
 				"1" 	        : Operation succeeded								       
 Info :																						           
 Note : Mandatory for all types except SRAM and PSRAM			   
********************************************************************************/
int Write (uint32_t Address, uint32_t Size, uint8_t* buffer)
{
	
  logPrintf("Write, addr 0x%X, %d\n", (int)Address, (int)Size);

	return 1;
}

/*******************************************************************************
 Description :																			
 Erase a full sector in the device 									
 Inputs :																					  
 				SectrorAddress	: Start of sector 					
 outputs :																				  
 				"1" : Operation succeeded										
 				"0" : Operation failure											
 Note : Not Mandatory for SRAM PSRAM and NOR_FLASH		
********************************************************************************/
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{      
	uint32_t BlockAddr;
	EraseStartAddress = EraseStartAddress -  EraseStartAddress % 0x10000;
	
	logPrintf("SectorErase, addr 0x%X, 0x%X\n", (int)EraseStartAddress, (int)EraseEndAddress);
	return 1;

	while (EraseEndAddress>=EraseStartAddress)
	{
		BlockAddr = EraseStartAddress & 0x0FFFFFFF;
		//QUADSPI_EraseSector( BlockAddr);
    EraseStartAddress += 0x10000;
	}
 	return 1;	
}

/*******************************************************************************
 Description :																			
 Read data from the device	 														
 Inputs :																					
 				Address 	: Write location  										
 				Size 		: Length in bytes 										
 				buffer 		: Address where to get the data to write		
 outputs :																				
 				"1" 		: Operation succeeded								
 				"0" 		: Operation failure										
 Note : Not Mandatory                               
********************************************************************************/	
int Read (uint32_t Address, uint32_t Size, uint8_t* Buffer)
{     	
	int i = 0;
	
  logPrintf("Read, addr 0x%X, %d\n", (int)Address, (int)Size);
  return 1;
	
	for (i=0; i < Size;i++)
	{ 
		*(uint8_t*)Buffer++ = *(uint8_t*)Address;
		Address ++;
	}
   
	return 1;     
} 

/*******************************************************************************
 Description :																		
 Verify the data 	 														    
 Inputs :																					
 				MemoryAddr 	: Write location  					
 				RAMBufferAddr 	: RAM Address		          
 				Size 		: Length in bytes 								
 outputs :																				
 				"0" 		: Operation succeeded						
 Note : Not Mandatory                             	
********************************************************************************/
int Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size)
{ 
	uint32_t VerifiedData = 0;
	Size*=4;
        
  logPrintf("Verify, addr 0x%X, 0x%X, %d\n", (int)MemoryAddr, (int)RAMBufferAddr, (int)Size);
  return 1;

  while (Size>VerifiedData)
  {
		if ( *(uint8_t*)MemoryAddr++ != *((uint8_t*)RAMBufferAddr + VerifiedData))
			return (MemoryAddr + VerifiedData);  
		
		VerifiedData++;  
  }
        
  return 0;
}
/*******************************************************************************
 Description :																		
 System initialization										        
 Inputs 	:																        
 				 None 																		
 outputs 	:																				
 				"1" 		: Operation succeeded						
 				"0" 		: Operation failure							
********************************************************************************/
int Init (void)
{ 
  hwInit();

  return 1;
}


