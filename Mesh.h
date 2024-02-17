#ifndef __MESH_H
#define __MESH_H

//Cac lenh doc thong qua cung loai cong to:
#define MESH_PING								0xB0 	//Ping Connection 
#define MESH_NETWORK_ID					0xB1 	//Set Network ID
#define MESH_ROUTE_ID						0xB2 	//Set Route ID
#define MESH_ROUTE_ID_RES       0xBD  //Set Route ID response
#define MESH_READ_METER 				0xB3	//Doc cong to
#define MESH_BROADCAST_ID 			0xB4 	//Lenh quang ba doc thong tin 
#define MESH_BROADCAST_LOCK 		0xB5	//Lenh doc du lieu billing
#define MESH_GET_DATA_DIRECT		0xB6	//Lenh doc cong to truc tiep
#define MESH_BROADCAST_PASSWORD 0xB7 	//Lenh quang ba set password AES
#define MESH_BROADCAST_BILLING 	0xB8 	//Lenh quang ba chot so lieu
#define MESH_BROADCAST_TIMEOUT 	0xB9 	//Lenh quang ba set noroute timeout
#define MESH_CONFIG_TXPOWER 		0xBA 	//Lenh dat cong xuat TX

#define MESH_RES(cmd_id) ((~cmd_id & 0xF0) | (cmd_id & 0x0F))	//Gia tri cua command id tra loi dua tren command id hoi
//#define min(X, Y)  ((X) < (Y) ? (X) : (Y))

#define WAIT_NOTHING 					0	//
#define WAIT_SEND_TRANSFER 		1	//Cho chuyen tiep lenh
#define WAIT_SEND_RESPONSE 		2	//Cho gui tra loi 1
#define WAIT_READ_METER 			3	//Cho doc cong to

#define MAX_LEVEL							0x05	//So tang toi da cua MESH No Route
//Vi tri cua cac truong trong packet truyen nhan:
#define MFRA_LEN 					0 	//Vi tri byte do dai cua packet
#define MFRA_HEADER				1 	//Vi tri Byte chua header lenh
#define MFRA_NETWORK				2 	//Vi tri Byte Network ID
#define MFRA_CMDID 					4 	//Vi tri byte Command ID
//Phan khai bao cau truc No Route:
#define MFRA_KEY_INDEX 				5 	//Lock key + Command Index
#define MFRA_LEVEL 					6 	//Level cua fram hien tai
#define MFRA_SERIAL					7 	// 4 Byte chua ID (serial) cua mesh dich
#define MFRA_REQ_ID 				11 	// 4 byte ID cua mesh thuc hien truy van du lieu
#define MFRA_RSSI 					15 	//Vi tri byte chua RSSI
#define MFRA_LQI 					16 	//
#define MFRA_DATA 					17 	//Dia chi bat dau cua phan data
#define MFRA_GROUP 					18 	//Dia chi bat dau cua phan group
//Phan khai bao cau truc Routed:
#define MFRA_RLEVEL					5 	//Index tang
#define MFRA_RMESH					6 	//Index MESH

//Cac lenh doc cong to truc tiep:
#define METER_PING 				0xA0	//Ping
#define METER_AT_COMMAND		0xA1 	//Giao tiep lenh voi module
#define METER_TRANSFER_CMD 	0xA2	//Lenh doc cong to truc tiep
#define METER_GET_BILLING		0xA3 	//Lenh doc chi so khong khoa key
#define METER_BROADCAST_ID 	0xA4 	//Lenh quang ba doc thong tin
#define METER_BROADCAST_NAK 	0xAB 	//Tra loi lenh quang ba doc thong tin so lieu da cu, cho cap nhat
#define METER_BROADCAST_LOCK 	0xA5	//Lenh doc du lieu billing co khoa key

#define METER_EVENT				0x8A	//
#define METER_VOLTAGE_CURRENT 0xA9	//
#define METER_SHOW_SERIAL		0xAF	//

#define IND_HEADER 				0
#define IND_SERIAL 				1
#define IND_CMD					5
#define IND_DATA				6
#define IND_CMD_KEY				6
#define IND_CMD_KEY2			10
#define IND_CMD_LEVEL			7
#define IND_RES_KEY				11
#define IND_RES_LEVEL			10
#define IND_FCS					12

#define RF_PACKET_LENGTH       67      //Do dai toi da cua 1 packet. Su dung FIFO 64 byte

/*
*************************************************************************************************************************************
* Cac ham xu ly du lieu dung chung duoc khai bao trong file "Commond.c"
*************************************************************************************************************************************
*/
extern char noroute_packet[];
extern uint8_t MeterType;			//Ma loai cong to

extern uint16_t billingResetNumber; //Chi muc lan chot so lieu

//ROUTE_FRAME_TYPE r_frame;			//Thong so frame hien tai dang xu ly
extern uint8_t r_frame_cmd_id;
extern uint32_t r_frame_serial;			//Gia tri serial trong frame nhan duoc
extern uint16_t r_frame_net_id;			//
extern char *r_frame_mesh_list;
extern uint8_t r_frame_mesh_ind;		//
extern uint8_t r_frame_level_ind;
extern uint8_t r_frame_data_len;
extern char *r_frame_data;
extern int8_t r_frame_rssi;
extern uint8_t r_frame_lqi;

//NOROUTE_FRAME_TYPE n_frame;		//Thong so frame hien tai dang xu ly
extern uint8_t n_frame_cmd_id;
extern uint32_t n_frame_serial;			//Gia tri serial trong frame nhan duoc
extern uint16_t n_frame_net_id;			//Gia tri network id trong frame nhan duoc
extern uint8_t n_frame_key;				//Gia tri network id trong frame nhan duoc
extern uint8_t n_frame_index;			//Gia tri network id trong frame nhan duoc
extern uint8_t n_frame_level;
extern uint8_t n_frame_data_len;
extern char *n_frame_data;
extern int8_t n_frame_rssi;
extern uint8_t n_frame_lqi;

extern uint32_t billingReady;
extern uint8_t mesh_process; 		//Luu lai lenh cuoi cung nhan duoc can xu ly

extern uint8_t noroute_last_key_index;

void Mesh_Init(void);
void Mesh_Process(void);

uint8_t NorouteCheckFrameOK(char *data);
uint8_t RouteCheckFrameOK(char* data);

uint8_t ReadMeterCommand(char* data_in, uint8_t* dat_len); //Xu doc du lieu cong to goi boi MASTER hoac MESH

uint8_t GELEX_Execute_Command(char* res_buf, uint64_t serial, uint8_t cmd_id); //Doc cong to GELEX
uint8_t STAR_Execute_Command(char* res_buf, uint64_t serial, uint8_t cmd_id);  //Doc cong to STAR
uint8_t HNET_Execute_Command(char* data_in, uint8_t *data_len); //Doc cong to HNET

uint8_t RouteExecuteCommand(char *packet);
void RouteTransferExecute(char *packet, int8_t direct);
void RF_Mesh_Process(void);

int LoRa_receiveMessage(char *rxBuffer);
void NoRouteResponse();
void NoRouteUpdateStateCounter(int state);

void PrintDebug(uint8_t id, char *data, uint8_t len); //In chuoi debug toi da 64 byte

typedef struct Node {
    char Name[100];
    int data;
    struct Node* next;
}Node;
// Hàm để tạo một node mới
Node* createNode(char *name, int data) ;
void appendOrUpdate(Node** head_ref, char *name, int data); 
bool checkFire(Node* head) ;
void printList(Node* node);
extern Node* head;

#endif /* __MESH_H */
