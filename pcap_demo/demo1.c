/*=========================================================================
  工程名称：	03-libpcap-demo
  组成文件：	03-libpcap-demo(接收多个数据包).c
  功能描述： 	通过回调函数的方式，来抓取多个网络数据包
  维护记录：	
				2015-04-02 v1.0		add by Mike
=========================================================================*/
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>

#define BUFSIZE 1514

struct ether_header
{
	unsigned char ether_dhost[6];	//目的mac
	unsigned char ether_shost[6];	//源mac
	unsigned short ether_type;		//以太网类型
};

/*******************************回调函数************************************/
void ethernet_protocol_callback(unsigned char *argument,const struct pcap_pkthdr *packet_heaher,const unsigned char *packet_content)
{
	unsigned char *mac_string;				//
	struct ether_header *ethernet_protocol;
	unsigned short ethernet_type;			//以太网类型
	printf("----------------------------------------------------\n");
	printf("%s\n", ctime((time_t *)&(packet_heaher->ts.tv_sec))); //转换时间
	ethernet_protocol = (struct ether_header *)packet_content;
	
	mac_string = (unsigned char *)ethernet_protocol->ether_shost;//获取源mac地址
	printf("Mac Source Address is %02x:%02x:%02x:%02x:%02x:%02x\n",*(mac_string+0),*(mac_string+1),*(mac_string+2),*(mac_string+3),*(mac_string+4),*(mac_string+5));
	mac_string = (unsigned char *)ethernet_protocol->ether_dhost;//获取目的mac
	printf("Mac Destination Address is %02x:%02x:%02x:%02x:%02x:%02x\n",*(mac_string+0),*(mac_string+1),*(mac_string+2),*(mac_string+3),*(mac_string+4),*(mac_string+5));
	
	ethernet_type = ntohs(ethernet_protocol->ether_type);//获得以太网的类型
	printf("Ethernet type is :%04x\n",ethernet_type);
	switch(ethernet_type)
	{
		case 0x0800:printf("The network layer is IP protocol\n");break;//ip
		case 0x0806:printf("The network layer is ARP protocol\n");break;//arp
		case 0x0835:printf("The network layer is RARP protocol\n");break;//rarp
		default:break;
	}
	usleep(800*1000);
}
/*=========================================================================
  函数名称：int main(int argc,char *argv[])  
  功能描述：通过使用libpcap接收一个数据包，然后对数据包进行解析
  参数传递：无
  返回数据：无
  维护记录：
			2015-04-02 v1.0		add by Mike
=========================================================================*/
int main(int argc, char *argv[])
{
	char error_content[100];	//出错信息
	pcap_t * pcap_handle;
	unsigned char *mac_string;				
	unsigned short ethernet_type;			//以太网类型
	char *net_interface = NULL;					//接口名字
	struct pcap_pkthdr protocol_header;
	struct ether_header *ethernet_protocol;
	
	//获取网络接口
	net_interface = pcap_lookupdev(error_content);
	if(NULL == net_interface)
	{
		perror("pcap_lookupdev");
		exit(-1);
	}

	pcap_handle = pcap_open_live(net_interface,BUFSIZE,1,0,error_content);//打开网络接口
		
	if(pcap_loop(pcap_handle,-1,ethernet_protocol_callback,NULL) < 0)
	{
		perror("pcap_loop");
	}
	
	pcap_close(pcap_handle);
	return 0;
}
