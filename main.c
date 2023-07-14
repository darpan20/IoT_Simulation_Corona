
#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/serial-line.h"
#include "dev/leds.h"
#include <string.h>
#include <stdio.h>
#include <random.h>
#include "dev/uart1.h"
#include "sys/log.h"    // for logging info
#define LOG_MODULE "TASK 1"
#define LOG_LEVEL LOG_LEVEL_INFO
#define SEND_INTERVAL ()
static linkaddr_t infect_addr =  {{ 0x01, 0x01, 0x01, 0x00, 0x01, 0x74, 0x12, 0x00 }};       //link layer addr
static linkaddr_t infect_addr1 =  {{ 0x03, 0x03, 0x03, 0x00, 0x03, 0x74, 0x12, 0x00 }};       //link layer addr
//static linkaddr_t infect_addr2 =  {{ 0x05, 0x05, 0x05, 0x00, 0x05, 0x74, 0x12, 0x00 }};       //link layer addr
static linkaddr_t infect_addr2 =  {{ 0x08, 0x08, 0x08, 0x00, 0x08, 0x74, 0x12, 0x00 }};       //link layer addr
static linkaddr_t infect_addr3 =  {{ 0x13, 0x13, 0x13, 0x00, 0x13, 0x74, 0x12, 0x00 }};       //link layer addr

static bool infected;
static bool only;
static bool contact;
static bool quarantined;
static bool recovered;
static unsigned long virus=1111111111;
static struct etimer t2;
static struct etimer infected_t;
static int random_time;
static int recover_time[20];
PROCESS(LED_process, "LED process");
AUTOSTART_PROCESSES(&LED_process);
static int k;


 #define SERIAL_BUF_SIZE 128
static int uart_rx_callback(unsigned char c) {
     // uint8_t u;

      static char rx_buf[SERIAL_BUF_SIZE];
      static int i=0;
     // printf("\nReceived vv %lu",(unsigned long)c);
     // u = (uint8_t)c;
     //printf("\nReceived %c\n",c);
     //return 0;


    rx_buf[i] = c; //when a new byte arrives, insert it into the buffer
i++;
if(i==17)
{    printf("received from database : %s\n",rx_buf);
     i=0;
     if(strncmp(rx_buf,"Contact is made",15)==0)
       {
          if(infected){
         leds_on(LEDS_YELLOW);
         leds_on(LEDS_RED);
         printf("QUARANTINED AND INFECTED,contact made with infected mote %c%c!!!!\n",rx_buf[15],rx_buf[16]);
         infected=true;
       }
       else{

         leds_on(LEDS_YELLOW);
         printf("QUARANTINED but not infected,contact made with infected mote %c%c!!!!\n",rx_buf[15],rx_buf[16]);
       }
         NETSTACK_MAC.off();  // mote turned off
         quarantined=true;
         contact=true;
          // mote turned off
          printf("quarantined in received %d\n",quarantined);
          etimer_set(&t2,180* CLOCK_SECOND);

      }
    // else {infected=false;}

     rx_buf[0]='\0';
}



    return 0;

}
void receive_led(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{

    static unsigned long key;
    //static int key2;
    key=*((unsigned long *)data);     //
    //key2=*((int *)data);     //
    if(key==1111111111 &&!recovered &&!infected){
      infected=true;
      only=true;
      printf(" virus received!!!! infected %d\n",infected);
      etimer_set(&infected_t,40  * CLOCK_SECOND);
      leds_on(LEDS_RED);
    }
    LOG_INFO("Received  %lu from ",key);
    LOG_INFO_LLADDR(src);
    LOG_INFO("\n");
    printf("infected in check %d\n",infected);
    if(!(linkaddr_cmp(&infect_addr, &linkaddr_node_addr)
    ||linkaddr_cmp(&infect_addr1, &linkaddr_node_addr)
    ||linkaddr_cmp(&infect_addr2, &linkaddr_node_addr)
    ||linkaddr_cmp(&infect_addr3, &linkaddr_node_addr)
    ||quarantined))
        {
          if(!(recovered||key==1111111111)){
    LOG_INFO_("CHECKING DATABASE\n");
    printf("check: %lu\n",key);
        }
      }

}


PROCESS_THREAD(LED_process, ev, data)
{

  static struct etimer t;

  static struct etimer recover_t;
  static struct etimer virus_t;
  static unsigned long beaks=1;

  static unsigned long rand;
  PROCESS_BEGIN();

  nullnet_buf = (uint8_t *)&rand;
  nullnet_len = sizeof(rand);
  nullnet_set_input_callback(receive_led);      //receiving msgs in nullnet
  uart1_init(BAUD2UBR(115200)); //set the baud rate as necessary
  uart1_set_input(uart_rx_callback); //set the callback function

  random_time=  ((random_rand()%(25))+10);
  printf("random time: start %d\n",random_time);

   etimer_set(&t,5  * CLOCK_SECOND);
   for(k=0;k<4;k++)
   { recover_time[k]=  ((random_rand()%(251))+420);
    printf("rrecover random time: start %d\n",recover_time[k]);
   }
   if(*(linkaddr_node_addr.u8)==1) etimer_set(&recover_t,recover_time[0]  * CLOCK_SECOND);
   if(*(linkaddr_node_addr.u8)==3) etimer_set(&recover_t,recover_time[1]  * CLOCK_SECOND);
   if(*(linkaddr_node_addr.u8)==8) etimer_set(&recover_t,recover_time[2]  * CLOCK_SECOND);
   if(*(linkaddr_node_addr.u8)==19) etimer_set(&recover_t,recover_time[3]  * CLOCK_SECOND);
   etimer_set(&virus_t,random_time  * CLOCK_SECOND);



    while(1) {

      if(infected && etimer_expired(&infected_t)&& only)
      {
        leds_on(LEDS_YELLOW);
        printf("infected but no contact made with infected mote");
        NETSTACK_MAC.off();  // mote turned off
        quarantined=true;
        etimer_set(&t2,180* CLOCK_SECOND);
      }
  if(linkaddr_cmp(&infect_addr, &linkaddr_node_addr)
  ||linkaddr_cmp(&infect_addr1, &linkaddr_node_addr)
  ||linkaddr_cmp(&infect_addr2, &linkaddr_node_addr)
  ||linkaddr_cmp(&infect_addr3, &linkaddr_node_addr))
  {
        if(etimer_expired(&recover_t)&&(!recovered))
        {
          NETSTACK_MAC.off();
          printf("INITIALLY INFECTD NODE %d IS QUARANTINED after knowing it is infected:\n",*(linkaddr_node_addr.u8));
          quarantined=true;
          printf("quarantined initial node %d\n",quarantined);
          leds_on(LEDS_YELLOW);
          etimer_reset(&recover_t);
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&recover_t));
          NETSTACK_MAC.on();
          printf("INITIALLY INFECTD NODE %d IS RECOVERED AND WORKING:\n",*(linkaddr_node_addr.u8));
          etimer_reset(&t);
          quarantined=false;
          infected=false;
          recovered=true;
          leds_off(LEDS_YELLOW);
          leds_off(LEDS_RED);
          leds_on(LEDS_GREEN);
        }
        if(etimer_expired(&virus_t)&&!recovered)
        {
          printf(" sending virus to all\n");
          nullnet_buf = (uint8_t *)&virus;
          nullnet_len = sizeof(virus);
          nullnet_set_input_callback(receive_led);
          NETSTACK_NETWORK.output(NULL);
          random_time=  ((random_rand()%(25))+10);
          printf("random time: in %d\n",random_time);
          etimer_set(&virus_t,random_time  * CLOCK_SECOND);
        }

  }

      rand =*(linkaddr_node_addr.u8)*1000000+beaks;
        printf("infected %d\n",infected);
    if(!quarantined)
      {

      if((linkaddr_cmp(&infect_addr, &linkaddr_node_addr)
      ||linkaddr_cmp(&infect_addr1, &linkaddr_node_addr)
      ||linkaddr_cmp(&infect_addr2, &linkaddr_node_addr)
      ||linkaddr_cmp(&infect_addr3, &linkaddr_node_addr))
      &&(!recovered)) {
        infected=true;
        leds_on(LEDS_RED);
        //infected--;
        // LOG_INFO("infected after: %d  ",infected);
        // LOG_INFO_("\n");
        printf("Infected! Sending beacons to database as well as other nodes:\n");
        printf("%lu\n",rand);

      }
      else{
        if(!recovered) printf("Non-infected!!! Sending beacons only to other nodes: %lu \n",rand);
        else printf("Already recovered, not sending or beacons any more, and can't be infected now!!!");

    }
      // LOG_INFO("Sending %lu to ",rand);
      // LOG_INFO_LLADDR(NULL);
      // LOG_INFO_("\n");
      nullnet_buf = (uint8_t *)&rand;
      nullnet_len = sizeof(rand);
      nullnet_set_input_callback(receive_led);
      if(!recovered) NETSTACK_NETWORK.output(NULL);
       PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&t));
       etimer_reset(&t);

       beaks++;

    //  //printf("leds are off at node 1!!!!\n");
     }
     else
     {
       printf("quarantined can't send %d\n",quarantined);
       PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&t2));
       NETSTACK_MAC.on();
       printf("INFECTED NODE %d IS RECOVERED AND WORKING:\n",*(linkaddr_node_addr.u8));
       recovered=true;
       infected=false;
       quarantined=false;
       leds_off(LEDS_YELLOW);
       leds_off(LEDS_RED);
       leds_on(LEDS_GREEN);
       //timer_reset(&t2);
       etimer_reset(&t);

   }


   }


  PROCESS_END();
}

















//
// PT_THREAD(generate_routes(struct httpd_state *s))
// {
//   char buff[15];
//   PSOCK_BEGIN(&s->sout);
//   int temperature = 15 + rand() % 25;
//   sprintf(buff,"{\"temp\":%u}", temperature);
