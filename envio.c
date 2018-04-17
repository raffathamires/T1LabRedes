	/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - envio de mensagens com struct          */
/*-------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include "checksum.h"

#define IP4_HDRLEN 20
#define UDP_HDRLEN 8

unsigned char buff[1500];
int tx_len = 0;
char data[1500];

unsigned short in_cksum(unsigned short *addr,int len)
{
        register int sum = 0;
        u_short answer = 0;
        register u_short *w = addr;
        register int nleft = len;

        /*
         * Our algorithm is simple, using a 32 bit accumulator (sum), we add
         * sequential 16 bit words to it, and at the end, fold back all the
         * carry bits from the top 16 bits into the lower 16 bits.
         */
        while (nleft > 1)  {
                sum += *w++;
                nleft -= 2;
        }

        /* mop up an odd byte, if necessary */
        if (nleft == 1) {
                *(u_char *)(&answer) = *(u_char *)w ;
                sum += answer;
        }

        /* add back carry outs from top 16 bits to low 16 bits */
        sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
        sum += (sum >> 16);                     /* add carry */
        answer = ~sum;                          /* truncate to 16 bits */
        return(answer);
}

//falha na compilacao
void monta_tcp()
{

printf("estou no tcp");
fflush( stdout );
struct tcphdr *tcph = (struct tcphdr *) (buff + sizeof(struct iphdr) + sizeof(struct ether_header));

//tcph->th_sport = htons(6666);
//tcph->th_dport = htons(443);
//tcph->th_seq = 0X39A032DF;
//tcph->th_ack = 0XEEA4822F;
//tcph->th_off = 0;
//tcph->res1 = 0;
//tcph->doff = 0;
//tcph->fin = 0;
//tcph->syn = 1;
//tcph->rst = 0;
//tcph->psh = 0;
//tcph->ack = 0;
//tcph->urg = 0;
//tcph->res2 = 0;
//tcph->window = 0;
//tcph->check = 0;
//tcph->urg_ptr=0;

}


//montagem do header do UDP
//nao está montando no wireshark
void monta_udp()
{


printf("estou no udp");
fflush( stdout );
struct udphdr *udph = (struct udphdr *) (buff + sizeof(struct iphdr) + sizeof(struct ether_header));

udph->uh_sport = htons(6666);
udph->uh_dport = htons(8622);
udph->uh_ulen = htons(8);
udph->uh_sum = 0;

udph = (struct udphdr *) &buff[sizeof(struct iphdr)];
tx_len += sizeof(struct udphdr);

}


//montagem do cabecalho ipv4
void monta_ip()
{

int b = 0;

printf("Digite 1 para UDP ou 2 para TCP; \n");
fflush(stdout);
scanf("%d", &b);
fflush(stdin);

if(b == 1){
  printf("estou no ipv4 \n");
  fflush( stdout );
  struct iphdr *iph = (struct iphdr*)(buff + sizeof(struct ether_header));//
  char *ip_src = "10.32.143.158";
  char *ip_dst = "10.40.26.7";

  iph->version = 4;
  iph->ihl = 5;
  iph->tos = 0;
  iph->tot_len = htons(sizeof(struct iphdr) + sizeof (struct udphdr));
  //if(sizeof(struct iphdr) > 1486)
  //{
//	iph->tot_len = 1486;
  //} else {
	//iph->tot_len = sizeof(struct iphdr) + sizeof (struct udphdr);//tamanho do próprio ip + tamanho do cabeçalho do próximo
  //}
  iph->id = htons(54321);
  iph->frag_off = 0;
  iph->ttl = 0xFF;
  iph->protocol = IPPROTO_UDP;
  //checksum.h e o método fornecido nao estao funcionando
  iph->check = htons(in_cksum((unsigned short *) buff, sizeof(struct iphdr)));
  iph->saddr = htons(inet_addr(ip_src));
  iph->daddr = htons(inet_addr(ip_dst));

  iph = (struct iphdr *) &buff[sizeof(struct ether_header)];
  tx_len += sizeof(struct iphdr);

  //montagem do cabecalho UDP
  monta_udp();
} else if(b == 2){
  printf("estou no ipv4 - tcp\n");
  fflush( stdout );
  struct iphdr *iph = (struct iphdr*)(buff + sizeof(struct ether_header));
  char *ip_src = "10.32.143.158";
  char *ip_dst = "10.40.26.7";

  iph->version = 4;
  iph->ihl = 5;
  iph->tos = 0;
  iph->tot_len = 28;//sizeof(struct iphdr) + sizeof (struct udphdr);//tamanho do próprio ip + tamanho do cabeçalho do próximo
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 255;
  iph->protocol = 17;
  //checksum.h e o método fornecido nao estao funcionando
  iph->check = in_cksum((unsigned short *) buff, sizeof(struct iphdr));
  iph->saddr = inet_addr(ip_src);
  iph->daddr = inet_addr(ip_dst);

  iph = (struct iphdr *) &buff[sizeof(struct ether_header)];
  tx_len += sizeof(struct iphdr);

  //montagem do cabecalho TCP
  monta_tcp();
} else {
  printf("Você digitou: %d\n", b);
  fflush( stdout );
}

}

//parametros do netinet/ip6.h
void monta_ip6(){

int c = 0;

printf("Digite 1 para UDP ou 2 para TCP; \n");
fflush( stdout );
scanf("%d", &c);
fflush(stdin);

if(c == 1){
  struct ip6_hdr *ip6h = (struct ip6_hdr*)(buff + sizeof(struct ether_header));
}else if (c == 2){
  struct ip6_hdr *ip6h = (struct ip6_hdr*)(buff + sizeof(struct ether_header));
}else{
  printf("Você digitou: %d\n", c);
}

}

void monta_pacote()
{

	int a = 0;

	printf("Digite 4 para IPv4 ou 6 para IPv6; \n");
	fflush( stdout );
	scanf("%d", &a);
	fflush(stdin);

	if(a == 4){
	printf("vamos para o ipv4\n");
	fflush( stdout );
	  // as struct estao descritas nos seus arquivos .h
	  // por exemplo a ether_header esta no net/ethert.h
	  // a struct ip esta descrita no netinet/ip.h
	  struct ether_header *eth;

 	  // coloca o ponteiro do header ethernet apontando para a 1a. posicao do buffer
	  // onde inicia o header do ethernet.
	  eth = (struct ether_header *) &buff[0];

	  //Endereco Mac Destino
	  eth->ether_dhost[0] = 0X00;
	  eth->ether_dhost[1] = 0X06;
	  eth->ether_dhost[2] = 0X5B;
	  eth->ether_dhost[3] = 0X28;
	  eth->ether_dhost[4] = 0XAE;
	  eth->ether_dhost[5] = 0X73;

	  //Endereco Mac Origem
	  eth->ether_shost[0] = 0Xa4;
	  eth->ether_shost[1] = 0X1f;
	  eth->ether_shost[2] = 0X72;
	  eth->ether_shost[3] = 0Xf5;
	  eth->ether_shost[4] = 0X90;
	  eth->ether_shost[5] = 0X52;

 	  eth->ether_type = htons(0X800);//IPv4

	  //tamanho do cabecalho ethernet
	  tx_len += sizeof(struct ether_header);

	  //montagem do cabecalho do IPv4
 	  monta_ip();
	} else if(a == 6){
	printf("vamos para o ipv6\n");
	fflush( stdout );
	  // as struct estao descritas nos seus arquivos .h
	  // por exemplo a ether_header esta no net/ethert.h
	  // a struct ip esta descrita no netinet/ip.h
	  struct ether_header *eth;

 	  // coloca o ponteiro do header ethernet apontando para a 1a. posicao do buffer
	  // onde inicia o header do ethernet.
	  eth = (struct ether_header *) &buff[0];

	  //Endereco Mac Destino
	  eth->ether_dhost[0] = 0X00;
	  eth->ether_dhost[1] = 0X06;
	  eth->ether_dhost[2] = 0X5B;
	  eth->ether_dhost[3] = 0X28;
	  eth->ether_dhost[4] = 0XAE;
	  eth->ether_dhost[5] = 0X73;

	  //Endereco Mac Origem
	  eth->ether_shost[0] = 0Xa4;
	  eth->ether_shost[1] = 0X1f;
	  eth->ether_shost[2] = 0X72;
	  eth->ether_shost[3] = 0Xf5;
	  eth->ether_shost[4] = 0X90;
	  eth->ether_shost[5] = 0X52;

 	  eth->ether_type = htons(0x86DD);//IPv6

	  //tamanho do cabecalho ethernet
	  tx_len += sizeof(struct ether_header);

	  //montagem do cabecalho do IPV6
	  monta_ip6(); 
	} else {
	  printf("Você digitou: %d\n", a);
	}	
}

int main(int argc,char *argv[])
{
	int sock, i;
	struct ifreq ifr;
	struct sockaddr_ll to;
	socklen_t len;
	unsigned char addr[6];

    /* Inicializa com 0 os bytes de memoria apontados por ifr. */
	memset(&ifr, 0, sizeof(ifr));

    /* Criacao do socket. Uso do protocolo Ethernet em todos os pacotes. D� um "man" para ver os par�metros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
	if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)  {
		printf("Erro na criacao do socket.\n");
        exit(1);
 	}
	
	printf("print inicial\n");
	fflush( stdout );
	/* Identicacao de qual maquina (MAC) deve receber a mensagem enviada no socket. */
	to.sll_protocol= htons(ETH_P_ALL);
	
	//mudei essa linha
	//tava estatico '2' de atributo e coloquei o ifr.ifr_ifindex
	
	to.sll_ifindex = 2;//ifr.ifr_ifindex;
	
	/* indice da interface pela qual os pacotes serao enviados */
	memcpy (to.sll_addr, addr, 6);
	len = sizeof(struct sockaddr_ll);
	//na outra versão tá:
	//to.sll_halen = 6;
	to.sll_halen = ETH_ALEN;

	printf("vamos comecar com o ethernet\n");
	fflush( stdout );
	monta_pacote();


	while(1){
	if(sendto(sock, (char *) buff, sizeof(buff), 0, (struct sockaddr*) &to, len)<0)
			printf("sendto maquina destino.\n");
	}
}