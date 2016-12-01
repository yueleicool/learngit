#include <sys/types.h>  
#include <arpa/inet.h>  
#include <pcap.h>    
#include <time.h>    
#include <stdlib.h>    
#include <stdio.h>    
 
struct tcp_header{  
  u_int16_t sport;  
  u_int16_t dport;  
  u_int32_t sn;  
  u_int32_t ack;  
//  u_int16_t other;  
#if BYTE_ORDER == LITTLE_ENDIAN  
  u_char th_x2:4,  
    th_off:4;  
#endif  
#if BYTE_ORDER == BIG_ENDIAN  
  u_char th_off:4,  
    th_x2:4;  
#endif  
  u_char th_flags;  
#define TH_FIN 0x01  
#define TH_SYN 0x02  
#define TH_RST 0x04  
#define TH_PUSH 0x08  
#define TH_ACK 0x10  
#define TH_URG 0x20  
 
  u_int16_t win_size;  
  u_int16_t checksum;  
  u_int16_t urg_ptr;  
};  
 
void tcp_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr *packet_header, const u_char *packet_content)  
{  
  struct tcp_header *tcp_protocol;  
  tcp_protocol=(struct tcp_header*)(packet_content + 14 + 20);  
  u_short source_port=ntohs(tcp_protocol->sport);  
  u_short destination_port=ntohs(tcp_protocol->dport);  
  u_int sequence = ntohl(tcp_protocol->sn);  
  u_int acknowledgement = ntohl(tcp_protocol->ack);  
  u_char flags = tcp_protocol->th_flags;  
  u_int16_t win_size = ntohl(tcp_protocol->win_size);  
  u_int16_t checksum = ntohl(tcp_protocol->checksum);  
  printf("Tcp Source Port:%d\n",source_port);  
  printf("Tcp Destination Port:%d\n",destination_port);  
  printf("Sequence Number:%u\n",sequence);  
  printf("Acknowledgement Number:%u\n",acknowledgement);  
  printf("Flags:");  
  if(flags & 0x08)   printf("PSH ");  
  if(flags & 0x10)   printf("ACK ");  
  if(flags & 0x02)   printf("SYN ");  
  if(flags & 0x20)   printf("URG ");  
  if(flags & 0x01)   printf("FIN ");  
  if(flags & 0x04)   printf("RST ");  
  printf("\n");  
 
  int i;  
  for(i=0; i<packet_header->len; ++i)  
  {  
    printf(" %02X", packet_content[i]);  
    if( (i + 1) % 16 == 0 )  
    {  
      printf("\n");  
    }  
  }  
 
} 
