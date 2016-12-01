#include <sys/types.h>  
#include <arpa/inet.h>  
#include <pcap.h>    
#include <time.h>    
#include <stdlib.h>    
#include <stdio.h>    
 
struct ether_header{  
  u_int8_t ether_dhost[6];  
  u_int8_t ether_shost[6];  
  u_int16_t ether_type;  
};  
 
void getPacket(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet)    
{   
  struct ip_header *ip_protocol;   
  struct ether_header *ethernet_protocol;  
  int * id = (int *)arg;    
  u_char *mac_string;  
    
  ethernet_protocol=(struct ether_header*)packet;  
  u_short ethernet_type = ntohs(ethernet_protocol->ether_type);  
 
  switch (ethernet_type){  
    case 0x0800:  
      printf("The network layer is IP protocol\n");  
      break;  
    case 0x0806:  
      printf("The network layer is ARP protocol\n");  
      break;  
    case 0x8035:  
      printf("The network layer is RAPR protocol\n");  
      break;  
    default:  
      printf("The network laye protocol unknow\n");  
      break;  
  }  
    
  printf("Mac Source Address is :\n");  
  mac_string = ethernet_protocol->ether_shost;  
  printf("%02x:%02x:%02x:%02x:%02x:%02x\n",*mac_string,*(mac_string+1),*(mac_string+2),*(mac_string+3),*(mac_string+4),*(mac_string+5));  
  printf("id: %d\n", ++(*id));  
  printf("Packet length: %d\n", pkthdr->len);    
  printf("Number of bytes: %d\n", pkthdr->caplen);    
  printf("Recieved time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec));     
  switch(ethernet_type){  
    case 0x0800:  
      ip_protocol_packet_callback(arg,pkthdr,packet);  
      break;  
    default:  
      break;  
  }  
  printf("\n\n");    
} 

