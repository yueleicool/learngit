#include <sys/types.h>  
#include <arpa/inet.h>  
#include <pcap.h>    
#include <time.h>    
#include <stdlib.h>    
#include <stdio.h>    
#include "lib.h"   
    
int main(int argc,char *argv[])    
{    
  char errBuf[PCAP_ERRBUF_SIZE], * devStr;  
 
  if(argc>1){  
    devStr = argv[1];  
  }  
  else{   
  /* get a device */    
    devStr = pcap_lookupdev(errBuf);    
  }  
 
  if(devStr)    
  {    
    printf("success: device: %s\n", devStr);    
  }    
  else    
  {    
    printf("error: %s\n", errBuf);    
    exit(1);    
  }    
      
  /* open a device, wait until a packet arrives */    
  pcap_t * device = pcap_open_live(devStr, 65535, 1, 0, errBuf);    
      
  if(!device)    
  {    
    printf("error: pcap_open_live(): %s\n", errBuf);    
    exit(1);    
  }    
      
  /* construct a filter */    
  struct bpf_program filter;    
  pcap_compile(device, &filter, "port 80", 1, 0);    
  pcap_setfilter(device, &filter);    
      
  /* wait loop forever */    
  int id = 0;    
  pcap_loop(device, -1, getPacket, (u_char*)&id);    
      
  pcap_close(device);    
    
  return 0;    
} 
