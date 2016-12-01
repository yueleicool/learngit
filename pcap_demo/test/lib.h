struct ether_header;  
struct ip_header;  
struct tcp_header;  
 
void tcp_protocol_packet_callback(u_char *, const struct pcap_pkthdr *, const u_char *);  
void ip_protocol_packet_callback(u_char *, const struct pcap_pkthdr *, const u_char *);  
void getPacket(u_char *, const struct pcap_pkthdr *, const u_char *); 
