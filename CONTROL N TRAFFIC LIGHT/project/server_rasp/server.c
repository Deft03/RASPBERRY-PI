
// Trần Thanh Sang 20146412 (Nhóm chiều t6)
// Thạch Phi Sách 20146411
// Trần Thái Bảo 20146054

//RUN CODE
// gcc server.c $(mariadb_config --libs) -o server $(mariadb_config --cflags) -lpaho-mqtt3c -lm


#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>
#include <stdint.h>

#define ADDRESS     "mqtt-dashboard.com"
#define CLIENTID    "publisher_demo"
#define PUB_TOPIC   "k20" 
#define SUB_TOPIC   "k21"   

uint8_t mode1,mode2 ; // mode tuong ung (1:red 2:green 3:yellow)

uint16_t id1,id2;    // stt cua den

uint32_t count1, count2;  // bien dem thoi gian con lai cua den1,den2

uint32_t t_red1,t_green1,t_yellow1 ;
uint32_t t_red2,t_green2,t_yellow2 ;

uint32_t id, count, mode;  // bien data nhận về

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
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    // vd: token = 
    char *token = strtok(message->payload, ",");  // nhan data va tach data theo dau phay id,count,mode
    sscanf(token, "Id:%d&Count:%d&Mode:%d",&id,&count,&mode);

    printf("Received message: id: %d,count: %d mode: %d,\n",id,count,mode);
    if(id==1){
        id1=1;
        count1 = count; 
        mode1 =mode;
    }
    if(id==2){
        id2=2;
        count2 =count;
        mode2 = mode;
    }

    // Connect to database
    MYSQL *conn;
    char *server = "localhost"; 
    char *user = "Sang";
    char *password = "123"; /* set me first */
    char *database = "bai2";

    conn = mysql_init(NULL);
    // ktra mysql
    if (mysql_real_connect(conn, server, user, password, database, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }  
    ///tao bien char de gui data xuong mysql
    char sql1[200];
    char sql2[200];

    sprintf(sql1,"update monitor1 set count=%d, mode=%d where id=%d",count1, mode1, id1);  
    sprintf(sql2,"update monitor1 set count=%d, mode=%d where id=%d",count2, mode2, id2);
    /// gui data
    mysql_query(conn,sql1);
    mysql_query(conn,sql2);
    mysql_close(conn); //close mysql
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;  
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    //connect sql
    MYSQL *conn;
    MYSQL_RES *res; // variable used to store DB data
    MYSQL_ROW row;
    char *server = "localhost"; 
    char *user = "Sang";
    char *password = "123"; /* set me 2 */
    char *database = "bai2";
  
    //listen for operation
    MQTTClient_subscribe(client, SUB_TOPIC, 0);

    while(1) {
        // Connect to database
        conn = mysql_init(NULL);
        mysql_real_connect(conn,server,user,password,database,0,NULL,0);
        // Read data from database
        mysql_query(conn, "select * from control");
        res = mysql_store_result(conn);
        int num_row = mysql_num_rows(res);
        //printf("%d",num_row);
        while (row = mysql_fetch_row(res)){
            for(int i = 1; i < num_row; i++){
                char msg [200];
                sprintf(msg,"%s,%s,%s,%s",row[0],row[1],row[2],row[3]); // gui data format: id,time_red,time_green,time_yellow
                publish(client, PUB_TOPIC, msg);
            }
            // sau 2s thi gui data di 1 lan
            sleep(2);
        }
    // clear result and close the connection
    mysql_free_result(res);
    mysql_close(conn);
    }
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}