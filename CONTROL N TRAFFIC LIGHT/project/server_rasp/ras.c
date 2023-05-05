// Trần Thanh Sang 20146412  ( nhóm chiều thứ 6)
// Thạch Phi Sách 20146411
// Trần Thái Bảo 20146054

// RUN CODE: // gcc -o ras ras.c -lwiringPi -lpthread -lmysqlclient -lpaho-mqtt3c -lm
/* Include Function  -----------------------------------------------------------------------------------*/
#include <mysql/mysql.h>
#include <wiringPi.h>
#include "stdlib.h"
#include <softPwm.h>
#include <stdint.h>
#include "string.h"
#include "unistd.h"
#include "MQTTClient.h"
#include <math.h>
#include <pthread.h>
/* End Include Function  -----------------------------------------------------------------------------------*/

/* Define  -----------------------------------------------------------------------------------*/
#define ADDRESS     "mqtt-dashboard.com"
#define CLIENTID    "subscribe_demo"    
#define PUB_TOPIC   "k21" ///// pub topic 
#define SUB_TOPIC   "k20" ///// sub topic 
// port 
#define red1     17
#define green1   27
#define yellow1  22

#define red2     18
#define green2   23
#define yellow2  24

uint8_t led[6] = {red1, green1, yellow1, red2, green2, yellow2};  // Khai báo output

uint16_t id1,id2;  // stt cua den
int count1, count2;

uint8_t mode1 =0,mode2 =0;

uint32_t t_red1,t_green1,t_yellow1 ;
uint32_t t_red2,t_green2,t_yellow2 ;

// #define QOS         1

/* END Define  -----------------------------------------------------------------------------------*/

/* SEND DATA FUNCTION FOR RASP  -----------------------------------------------------------------------------------*/
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    printf("Message '%s' with delivery token %d delivered\n", payload, token);
} 

/* Recieve DATA FUNCTION  -----------------------------------------------------------------------------------*/
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    //vd: token = 1,1,2,3
    char *token = strtok(message->payload, ",");  // tach data theo format id,red,green,blue
    int id = atoi(token);  // atoi chuyen kieu char --> int
    token = strtok(NULL, ","); //phat hien dau phay
    int red = atoi(token);
    token = strtok(NULL, ",");
    int green = atoi(token);
    token = strtok(NULL, ",");
    int yellow = atoi(token);

    // Gán các biến vs id tương ứng
    if(id==1){
        id1 = 1;
        t_green1 =green; 
        t_red1 = red;
        t_yellow1 = yellow;  

    }
    if(id==2){
        id2 = 2;
        t_green2 =green;
        t_red2 = red;
        t_yellow2 = yellow;  
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
// Hàm tắt mở đèn với thời gian tương ứng
void light1(int t_light, int addr){
    for( count1=t_light; count1>=0;count1--){
        softPwmWrite(addr, 100);
        delay(1000);
        }
    softPwmWrite(addr, 0);

}
void light2(int t_light, int addr){
    for( count2=t_light; count2>=0;count2--){
        softPwmWrite(addr, 100);
        delay(1000);
        }
    softPwmWrite(addr, 0);

}
// Ham xu ly cho thread 1
void *Thread1(void *threadid){
    while(1){
        mode1=1;
        light1(t_red1, red1); 
        mode1=2;
        light1(t_green1, green1);   
        mode1=3;
        light1(t_yellow1, yellow1);        
    }

}
// Ham xu ly cho thread 2
void *Thread2(void *threadid){
    while(1){
        mode2=1;
        light2(t_red2, red2); 
        mode2=2;
        light2(t_green2, green2);   
        mode2=3;
        light2(t_yellow2, yellow2);
    }
    
}

/* Main FUNCTION  -----------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {

    wiringPiSetupGpio();

    int sl =2;      // Số lượng đèn cần điều khiển độc lập : 2
    pthread_t threads[sl]; 

    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    //conn_opts.username = "your_username>>";
    //conn_opts.password = "password";

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    ////// Khai bao chan ở chế độ output
    int rc, i = 0;
    for(int i=0; i<6; i++){
		pinMode(led[i], OUTPUT);
		softPwmCreate(led[i], 0 , 100);
		}

    // Tap thread 1
    printf("Creating thead %d, \n",i);
    if ( (rc = pthread_create(&threads[i], NULL, Thread1, NULL)) ){
        printf("Error:unable to create thread, %d\n",rc );
        return 0;
    }
    i++;
    // Tao thread 2
    printf("Creating thead %d, \n",i);
    if ( (rc = pthread_create(&threads[i], NULL, Thread2, NULL)) ){
        printf("Error:unable to create thread, %d\n",rc );
        return 0;
    }

    // Kien tra ket noi mqtt
    int rd;
    if ((rd = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rd);
        exit(-1);
    }
   
    //listen for operation
    MQTTClient_subscribe(client, SUB_TOPIC, 0);
	
    while(1) {
        char msg1 [200];
        char msg2 [200];
        
        sprintf(msg1,"Id:%d&Count:%d&Mode:%d",id1,count1,mode1);
        sprintf(msg2,"Id:%d&Count:%d&Mode:%d",id2,count2,mode2);

        publish(client, PUB_TOPIC, msg1);
        publish(client, PUB_TOPIC, msg2);
        
        millis();
    }
	 
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rd;
    pthread_exit(NULL);
}