#include <libnet.h>
//ARP广播包:

int main() {
    libnet_t *handle;        /* Libnet句柄 */
    int packet_size;
    char *device = "eth0";   /* 设备名字,也支持点十进制的IP地址,会自己找到匹配的设备 */
    u_int8_t *src_ip_str = "192.168.128.200";       /* 源IP地址字符串 */
    u_int8_t *dst_ip_str = "192.168.128.88";        /* 目的IP地址字符串 */
    u_int8_t src_mac[6] = {0x00, 0x0c, 0x29, 0x73, 0xfa, 0x86};/* 源MAC */
    u_int8_t dst_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};/* 目的MAC,广播地址 */
    /* 接收方MAC,ARP请求目的就是要询问对方MAC,所以这里填写0 */
    u_int8_t rev_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    u_int32_t dst_ip, src_ip;              /* 网路序的目的IP和源IP */
    char error[LIBNET_ERRBUF_SIZE];        /* 出错信息 */
    libnet_ptag_t arp_proto_tag, eth_proto_tag;

    /* 把目的IP地址字符串转化成网络序 */
    dst_ip = libnet_name2addr4(handle, dst_ip_str, LIBNET_RESOLVE);
    /* 把源IP地址字符串转化成网络序 */
    src_ip = libnet_name2addr4(handle, src_ip_str, LIBNET_RESOLVE);

    if ( dst_ip == -1 || src_ip == -1 ) {
        printf("ip address convert error\n");
        exit(-1);
    };
    /* 初始化Libnet,注意第一个参数和TCP初始化不同 */
    if ( (handle = libnet_init(LIBNET_LINK_ADV, device, error)) == NULL ) {
        printf("libnet_init: error [%s]\n", error);
        exit(-2);
    };

    /* 构造arp协议块 */
    arp_proto_tag = libnet_build_arp(
                ARPHRD_ETHER,        /* 硬件类型,1表示以太网硬件地址 */ 
                ETHERTYPE_IP,        /* 0x0800表示询问IP地址 */ 
                6,                   /* 硬件地址长度 */ 
                4,                   /* IP地址长度 */ 
                ARPOP_REQUEST,       /* 操作方式:ARP请求 */ 
                src_mac,             /* source MAC addr */ 
                (u_int8_t *)&src_ip, /* src proto addr */ 
                rev_mac,             /* dst MAC addr */ 
                (u_int8_t *)&dst_ip, /* dst IP addr */ 
                NULL,                /* no payload */ 
                0,                   /* payload length */ 
                handle,              /* libnet tag */ 
                0                    /* Create new one */
    );
    if (arp_proto_tag == -1)    {
        printf("build IP failure\n");
        exit(-3);
    };

    /* 构造一个以太网协议块
    You should only use this function when 
    libnet is initialized with the LIBNET_LINK interface.*/
    eth_proto_tag = libnet_build_ethernet(
        dst_mac,         /* 以太网目的地址 */
        src_mac,         /* 以太网源地址 */
        ETHERTYPE_ARP,   /* 以太网上层协议类型，此时为ARP请求 */
        NULL,            /* 负载，这里为空 */ 
        0,               /* 负载大小 */
        handle,          /* Libnet句柄 */
        0                /* 协议块标记，0表示构造一个新的 */ 
    );
    if (eth_proto_tag == -1) {
        printf("build eth_header failure\n");
        return (-4);
    };

    packet_size = libnet_write(handle);    /* 发送已经构造的数据包*/

    libnet_destroy(handle);                /* 释放句柄 */

    return (0);
}
