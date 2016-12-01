#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
#define SNAP_LEN 1518       // 以太网帧最大长度
#define SIZE_ETHERNET 14   // 以太网包头长度 mac 6*2, type: 2
#define ETHER_ADDR_LEN  6  // mac地址长度
 
 
struct packet_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP? ARP? RARP? etc */
};
 
/* IP header */
struct packet_ip {
    u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char  ip_tos;                 /* type of service */
    u_short ip_len;                 /* total length */
    u_short ip_id;                  /* identification */
    u_short ip_off;                 /* fragment offset field */
    #define IP_RF 0x8000            /* reserved fragment flag */
    #define IP_DF 0x4000            /* dont fragment flag */
    #define IP_MF 0x2000            /* more fragments flag */
    #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    u_char  ip_ttl;                 /* time to live */
    u_char  ip_p;                   /* protocol */
    u_short ip_sum;                 /* checksum */
    struct  in_addr ip_src,ip_dst;  /* source and dest address */
    //struct in_addr ip_src;
    //struct in_addr ip_dst;              /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)
 
/* TCP header */
typedef u_int tcp_seq;
 
struct packet_tcp {
    u_short th_sport;               /* source port */
    u_short th_dport;               /* destination port */
    tcp_seq th_seq;                 /* sequence number */
    tcp_seq th_ack;                 /* acknowledgement number */
    u_char  th_offx2;               /* data offset, rsvd */
    #define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
    u_char  th_flags;
    #define TH_FIN  0x01
    #define TH_SYN  0x02
    #define TH_RST  0x04
    #define TH_PUSH 0x08
    #define TH_ACK  0x10
    #define TH_URG  0x20
    #define TH_ECE  0x40
    #define TH_CWR  0x80
    #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;                 /* window */
    u_short th_sum;                 /* checksum */
    u_short th_urp;                 /* urgent pointer */
};
 
 
void loop_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    static int count = 0;                   // 包计数器
    const struct packet_ethernet *ethernet;  /* The ethernet header [1] */
    const struct packet_ip *ip;              /* The IP header */
    const struct packet_tcp *tcp;            /* The TCP header */
    const char *payload;                     /* Packet payload */
 
    int size_ip;
    int size_tcp;
    int size_payload;
 
    count++;
 
    /* 以太网头 */
    ethernet = (struct packet_ethernet*)(packet);
 
    /* IP头 */
    ip = (struct packet_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20) {
        printf("无效的IP头长度: %u bytes\n", size_ip);
        return;
    }
 
    if ( ip->ip_p != IPPROTO_TCP ){ // TCP,UDP,ICMP,IP
    return;
    }
 
    /* TCP头 */
    tcp = (struct packet_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("无效的TCP头长度: %u bytes\n", size_tcp);
        return;
    }
 
    int sport =  ntohs(tcp->th_sport);
    int dport =  ntohs(tcp->th_dport);
 
    printf("%s:%d -> ", inet_ntoa(ip->ip_src), sport);
    printf("%s:%d ", inet_ntoa(ip->ip_dst), dport);
 
    //内容
    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
 
    //内容长度 
    size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
 
    if (size_payload > 0) {
        //printf("%d bytes:\n", size_payload, payload);
    printf("%d %d %d %d bytes\n", ntohs(tcp->th_seq), ntohs(tcp->th_ack), ntohs(tcp->th_flags), size_payload );
    write(payload, size_payload);
    } else {
    printf("%d %d %d \n", ntohs(tcp->th_seq), ntohs(tcp->th_ack), ntohs(TH_SYN));
    }
}
 
 
int write(const u_char *p, int len ){
    FILE *fp;
    fp = fopen("/opt/mm/bin","a");
    fwrite(p, len, 1, fp );
    fwrite("\n\n", 4, 1, fp );
    fclose(fp);
}
 
void aloop_callback(u_char* argument, const struct pcap_pkthdr* header, const u_char* content){
    write( content, header->caplen );
}
 
int main(int argc, char *argv[]) {
    pcap_t *handle; /* 会话句柄 */
    char *dev; /* 执行嗅探的设备 */
    char errbuf[PCAP_ERRBUF_SIZE]; /* 存储错误信息的字符串 */
    struct bpf_program filter; /* 已经编译好的过滤器 */
    char filter_app[] = "port 80"; /* 过滤表达式 */
    bpf_u_int32 mask; /* 所在网络的掩码 */
    bpf_u_int32 net; /* 主机的IP地址 */
    struct pcap_pkthdr header; /* 由pcap.h定义 */
    const u_char *packet; /* 实际的包 */
    /* Define the device */
    /* dev = pcap_lookupdev(errbuf); */
    dev = "em2";  /* 网卡名称 */
    pcap_lookupnet(dev, &net, &mask, errbuf); /* 探查设备属性 */
    handle = pcap_open_live(dev, 65536, 1, 0, errbuf); /* 以混杂模式打开会话 */
    pcap_compile(handle, &filter, filter_app, 0, net); /* 编译并应用过滤器 */
    pcap_setfilter(handle, &filter);
 
    pcap_loop( handle, 10, loop_callback, NULL);
    pcap_close(handle); /* 关闭会话 */
    return(0);
}
