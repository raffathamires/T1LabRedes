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
#include "checksum.h"

unsigned char buff[1500];
int tx_len = 0;

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

void monta_ip()
{

//char packet = [512];

struct iphdr *ip = (struct iphdr*)(buff + sizeof(struct ether_header));
char *ip_src = "10.32.143.187";
char *ip_dst = "10.40.26.7";

ip->version = 4;
ip->ihl = 5;
ip->tos = 0;
ip->tot_len = htons((sizeof(struct iphdr)));//tamanho do próprio ip + tamanho do cabeçalho do próximo
ip->id = 0;
ip->frag_off = 0;
ip->ttl = 3;
ip->protocol = 6;
ip->check = in_cksum((unsigned short*) ip, sizeof(struct ip));
ip->saddr = inet_addr(ip_src);
ip->daddr = inet_addr(ip_dst);

ip = (struct iphdr *) &buff[sizeof(struct ether_header)];

}

void monta_pacote()
{
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
	eth->ether_shost[0] = 0XA4;
	eth->ether_shost[1] = 0X1F;
	eth->ether_shost[2] = 0X72;
	eth->ether_shost[3] = 0XF5;
	eth->ether_shost[4] = 0X90;
	eth->ether_shost[5] = 0X4A;

 	eth->ether_type = htons(0X800);

	//tamanho do cabecalho ethernet
	tx_len += sizeof(struct ether_header);

	//montagem do cabecalho do IPv4
	monta_ip();
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

	/* Identicacao de qual maquina (MAC) deve receber a mensagem enviada no socket. */
	to.sll_protocol= htons(ETH_P_ALL);
	
	//mudei essa linha
	//tava estatico '2' de atributo e coloquei o ifr.ifr_ifindex
	
	to.sll_ifindex = ifr.ifr_ifindex;
	
	/* indice da interface pela qual os pacotes serao enviados */
	memcpy (to.sll_addr, addr, 6);
	len = sizeof(struct sockaddr_ll);
	//na outra versão tá:
	////to.sll_halen = 6;
	to.sll_halen = ETH_ALEN;

	monta_pacote();

	while(1){
	if(sendto(sock, (char *) buff, sizeof(buff), 0, (struct sockaddr*) &to, len)<0)
			printf("sendto maquina destino.\n");
	}
}

