/*=========================================================================
  工程名称：	01-libpcap-demo
  组成文件：	01-libpcap-demo(接收一个数据包).c
  功能描述： 	通过捕获一个网络数据包，然后对其进行数据的解析分析
  维护记录：	
				2015-04-02 v1.0		add by Mike
=========================================================================*/
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
struct ether_header
{
	unsigned char ether_dhost[6];	//目的mac
	unsigned char ether_shost[6];	//源mac
	unsigned short ether_type;		//以太网类型
};
#define BUFSIZE 1514

/*=========================================================================
  函数名称：int main(int argc,char *argv[])  
  功能描述：通过使用libpcap接收一个数据包，然后对数据包进行解析
  参数传递：无
  返回数据：无
  维护记录：
			2015-04-02 v1.0		add by Mike
=========================================================================*/
int main(int argc,char *argv[])
{
	pcap_t * pcap_handle = NULL;
	char error_content[100] = "";	// 出错信息
	const unsigned char *p_packet_content = NULL;		// 保存接收到的数据包的起始地址
	unsigned char *p_mac_string = NULL;			// 保存mac的地址，临时变量
	unsigned short ethernet_type = 0;			// 以太网类型
	char *p_net_interface_name = NULL;		// 接口名字
	struct pcap_pkthdr protocol_header;
	struct ether_header *ethernet_protocol;

	//获得接口名
	p_net_interface_name = pcap_lookupdev(error_content);
	if(NULL == p_net_interface_name)
	{
		perror("pcap_lookupdev");
		exit(-1);
	}
	
	//打开网络接口
	pcap_handle = pcap_open_live(p_net_interface_name,BUFSIZE,1,0,error_content);
	p_packet_content = pcap_next(pcap_handle,&protocol_header);
	
	printf("------------------------------------------------------------------------\n");
	printf("capture a Packet from p_net_interface_name :%s\n",p_net_interface_name);
	printf("Capture Time is :%s",ctime((const time_t *)&protocol_header.ts.tv_sec));
	printf("Packet Lenght is :%d\n",protocol_header.len);
	
	/*
	*分析以太网中的 源mac、目的mac
	*/
	ethernet_protocol = (struct ether_header *)p_packet_content;
	p_mac_string = (unsigned char *)ethernet_protocol->ether_shost;//获取源mac
	printf("Mac Source Address is %02x:%02x:%02x:%02x:%02x:%02x\n",*(p_mac_string+0),*(p_mac_string+1),*(p_mac_string+2),*(p_mac_string+3),*(p_mac_string+4),*(p_mac_string+5));
	p_mac_string = (unsigned char *)ethernet_protocol->ether_dhost;//获取目的mac
	printf("Mac Destination Address is %02x:%02x:%02x:%02x:%02x:%02x\n",*(p_mac_string+0),*(p_mac_string+1),*(p_mac_string+2),*(p_mac_string+3),*(p_mac_string+4),*(p_mac_string+5));

	/*
	*获得以太网的数据包的地址，然后分析出上层网络协议的类型
	*/
	ethernet_type = ntohs(ethernet_protocol->ether_type);
	printf("Ethernet type is :%04x\t",ethernet_type);
	switch(ethernet_type)
	{
		case 0x0800:printf("The network layer is IP protocol\n");break;//ip
		case 0x0806:printf("The network layer is ARP protocol\n");break;//arp
		case 0x0835:printf("The network layer is RARP protocol\n");break;//rarp
		default:printf("The network layer unknow!\n");break;
	}
	
	pcap_close(pcap_handle);
	return 0;
}

