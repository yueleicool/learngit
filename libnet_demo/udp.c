#include <libnet.h>

int main() {
    libnet_t *handle; /* Libnet句柄 */
    int packet_size; /* 构造的数据包大小 */
    char *device = "eth0"; /* 设备名字,也支持点十进制的IP地址,会自己找到匹配的设备 */
    char *src_ip_str = "192.168.2.148"; /* 源IP地址字符串 */
    char *dst_ip_str = "192.168.2.170"; /* 目的IP地址字符串 */
    u_char src_mac[6] = {0x00, 0x0c, 0x29, 0xba, 0xee, 0xdd}; /* 源MAC */
    u_char dst_mac[6] = {0x00, 0x0c, 0x29, 0x6d, 0x4d, 0x5c}; /* 目的MAC */
    u_long dst_ip, src_ip; /* 网路序的目的IP和源IP */
    char error[LIBNET_ERRBUF_SIZE]; /* 出错信息 */
    libnet_ptag_t eth_tag, ip_tag, udp_tag; /* 各层build函数返回值 */
    u_short proto = IPPROTO_UDP; /* 传输层协议 */
    u_char payload[255] = {0}; /* 承载数据的数组，初值为空 */
    u_long payload_s = 0; /* 承载数据的长度，初值为0 */

    /* 把目的IP地址字符串转化成网络序 */
    dst_ip = libnet_name2addr4(handle, dst_ip_str, LIBNET_RESOLVE);
    /* 把源IP地址字符串转化成网络序 */
    src_ip = libnet_name2addr4(handle, src_ip_str, LIBNET_RESOLVE);

    /* 初始化Libnet */
    if ( (handle = libnet_init(LIBNET_LINK, device, error)) == NULL ) {
        printf("libnet_init failure\n");
        return (-1);
    };

    strncpy(payload, "test", sizeof(payload)-1); /* 构造负载的内容 */
    payload_s = strlen(payload); /* 计算负载内容的长度 */

    udp_tag = libnet_build_udp(
                30330, /* 源端口 */
                30331, /* 目的端口 */
                LIBNET_UDP_H + payload_s, /* 长度 */
                0, /* 校验和,0为libnet自动计算 */
                payload, /* 负载内容 */
                payload_s, /* 负载内容长度 */
                handle, /* libnet句柄 */
                0 /* 新建包 */
    );
    if (udp_tag == -1) {
        printf("libnet_build_tcp failure\n");
        return (-3);
    };

    /* 构造IP协议块，返回值是新生成的IP协议快的一个标记 */
    ip_tag = libnet_build_ipv4(
        LIBNET_IPV4_H + LIBNET_UDP_H + payload_s, /* IP协议块的总长,*/
        0, /* tos */
        (u_short) libnet_get_prand(LIBNET_PRu16), /* id,随机产生0~65535 */
        0, /* frag 片偏移 */
        (u_int8_t)libnet_get_prand(LIBNET_PR8), /* ttl,随机产生0~255 */
        proto, /* 上层协议 */
        0, /* 校验和，此时为0，表示由Libnet自动计算 */
        src_ip, /* 源IP地址,网络序 */
        dst_ip, /* 目标IP地址,网络序 */
        NULL, /* 负载内容或为NULL */
        0, /* 负载内容的大小*/
        handle, /* Libnet句柄 */
        0 /* 协议块标记可修改或创建,0表示构造一个新的*/
    );
    if (ip_tag == -1) {
        printf("libnet_build_ipv4 failure\n");
        return (-4);
    };

    /* 构造一个以太网协议块,只能用于LIBNET_LINK */
    eth_tag = libnet_build_ethernet(
        dst_mac, /* 以太网目的地址 */
        src_mac, /* 以太网源地址 */
        ETHERTYPE_IP, /* 以太网上层协议类型，此时为IP类型 */
        NULL, /* 负载，这里为空 */ 
        0, /* 负载大小 */
        handle, /* Libnet句柄 */
        0 /* 协议块标记，0表示构造一个新的 */ 
    );
    if (eth_tag == -1) {
        printf("libnet_build_ethernet failure\n");
        return (-5);
    };

    packet_size = libnet_write(handle); /* 发送已经构造的数据包*/

    libnet_destroy(handle); /* 释放句柄 */

    return (0);
}
