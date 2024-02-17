#include "main.h"

////////////////////////////////////
///-Phan khai bao MESH khong dinh tuyen
////////////////////////////////////
//ROUTE_FRAME_TYPE r_frame;			//Thong so frame hien tai dang xu ly
uint8_t r_frame_cmd_id;
uint32_t r_frame_serial;			//Gia tri serial trong frame nhan duoc
uint16_t r_frame_net_id;			//
char *r_frame_mesh_list;
uint8_t r_frame_mesh_ind;		//
uint8_t r_frame_level_ind;
uint8_t r_frame_data_len;
char *r_frame_data;
int8_t r_frame_rssi;
uint8_t r_frame_lqi;

//NOROUTE_FRAME_TYPE n_frame;		//Thong so frame hien tai dang xu ly
uint8_t n_frame_cmd_id;
uint32_t n_frame_serial;			//Gia tri serial trong frame nhan duoc
uint16_t n_frame_net_id;			//Gia tri network id trong frame nhan duoc
uint8_t n_frame_key;				//Gia tri network id trong frame nhan duoc
uint8_t n_frame_index;			//Gia tri network id trong frame nhan duoc
uint32_t n_frame_serialReq;			//Gia tri serial quest trong frame nhan duoc
uint8_t n_frame_level;
uint8_t n_frame_data_len;
char *n_frame_data;
int8_t n_frame_rssi;
uint8_t n_frame_lqi;
uint8_t n_frame_group_id;
char n_nodeName[20]={0};

uint8_t MeterType = 0; //METER_TYPE_NONE;		//Ma loai cong to

uint32_t waitCCA = 0;     //Thoi gian cho ngau nhien tranh xung dot duong truyen
uint16_t lastCCA = 0;     //Thoi gian cho ngau nhien tranh xung dot duong truyen cua lan truoc
uint16_t NEXT_GAP = 10;

uint8_t noroute_last_key_index;

char noroute_packet[RF_PACKET_LENGTH]; 			//Bo dem lenh cho duoc gui, byte dau tien chua do dai, du lieu toi da 64 byte+1byte len+1rssi+1lqi

char rf_exec_cmd[RF_PACKET_LENGTH]; 		//Bo dem su dung boi thread cua timer RF Process
char rf_exec_res[RF_PACKET_LENGTH]; 			//

char meter_cmd[RF_PACKET_LENGTH]; 		//
char meter_res[RF_PACKET_LENGTH]; 			//

uint8_t mesh_process = 0; 		//Luu lai lenh cuoi cung nhan duoc can xu ly

uint16_t mesh_cmd_netid = 0;
uint8_t mesh_key_locked = 0;		//Key dang duoc khoa
uint8_t mesh_key_waiting = 0;	//Key se duoc khoa
uint8_t mesh_cmd_index = 0;
uint8_t mesh_res_index = 0;

uint8_t mesh_retry;	//Dem so lan truyen so lieu khong thanh cong de ket thuc qua trinh truyen
void copySubArray(uint8_t* source, uint8_t* destination, int start, int end) {
    int i, j = 0;

    // Lặp qua các phần tử từ vị trí bắt đầu đến vị trí kết thúc
    for (i = start; i <= end; ++i) {
        destination[j++] = source[i];
    }
}
/*CAC HAM SU LY NOROUTE*/
uint8_t NorouteCheckFrameOK(char* packet) { //Ham kiem tra tinh hop le cua frame nhan duoc qua RF
	//Frame RF chua do dai tai vi tri dau tien
	//Ket qua tra ve la NULL neu frame khong hop le, la dia chi cua chuoi bat dau frame
	//0  1                                                    17  
	//xx 24 15 01 B0 12 01 [00 00 04 98] [00 00 00 02] E2 00 [45]
	//if (level0_enable == 0) return 0;
  //Serial.printf("\r\n***Noroute frame checking ...");
  //Serial.printf("\r\n***Header=[%c]", packet[MFRA_HEADER]);
  //Serial.printf("\r\n***Length=[%c]", packet[MFRA_LEN]);
	if (packet[MFRA_HEADER] != (char)0x24) return 0;
	if (packet[MFRA_LEN] < (char)MFRA_DATA) return 0;
	//Serial.printf("\r\n*** 1");
	if (FCSADD_Check(&packet[MFRA_HEADER], packet[MFRA_LEN]) == 0) return 0;
	n_frame_serial = S2U32(&packet[MFRA_SERIAL],0,0);
	n_frame_cmd_id = packet[MFRA_CMDID];
	n_frame_net_id = S2U16(&packet[MFRA_NETWORK],0,0);
	n_frame_key = packet[MFRA_KEY_INDEX] & 0xF0;
	n_frame_index= packet[MFRA_KEY_INDEX] & 0x0F;
	n_frame_level = packet[MFRA_LEVEL];
	n_frame_data_len = packet[MFRA_LEN] - MFRA_DATA;
	//memcpy(data,&packet[MFRA_DATA],n_frame_data_len);
	n_frame_data = &packet[MFRA_DATA];
	n_frame_rssi = packet[packet[MFRA_LEN]+1];
	n_frame_lqi = packet[packet[MFRA_LEN]+2];
  n_frame_serialReq = S2U32(&packet[MFRA_REQ_ID],0,0);
  n_frame_group_id = packet[MFRA_GROUP] & 0x0F;
  
  copySubArray((uint8_t*)packet,(uint8_t*)n_nodeName,MFRA_GROUP+1,MFRA_GROUP+14);
  
  
  
  
  return 1;
}

void NoRouteTransfer() { //Chuan bi chuoi du lieu chuyen tiep
	if (m_cfg.serial > 0) { //Node phai co dia chi moi tra loi
    
		//FlashingRxLED(n_frame_rssi);
		noroute_packet[MFRA_LEVEL] += 1; //Tang so tang len 1
		U2S32(&noroute_packet[MFRA_REQ_ID], m_cfg.serial,0,0);
		if (noroute_packet[MFRA_LEN] > 61) noroute_packet[MFRA_LEN] = 61; //Cat bo frame neu dai qua gioi han
		FCSADD_Add(&noroute_packet[1],noroute_packet[0]); //Tinh toan FCS:
		mesh_retry = m_cfg.max_retry; //Reset bien dem so lan retry
		mesh_process = WAIT_SEND_TRANSFER; 
    //Serial.printf("\r\nNoroute transfer XXX\r\n");
	}
}


uint8_t NorouteProcess(char* packet) 
{
  char buff[RF_PACKET_LENGTH * 3];
	if (n_frame_serial == 0 // Dia chi 0 danh cho master
		|| (m_cfg.serial == 0) //Khong co dia chi se khong xu ly lenh
		) return 0; 
	//PrintDebug(0,packet,packet[0]+1);
	Serial.printf("\r\nNoroute Frame: Network=[%04X] Serial=[%08X] CMDID=[%02X] Index=[%02X] Level=[%02X]\r\n", n_frame_net_id, n_frame_serial, n_frame_cmd_id, n_frame_index, n_frame_level); //For debug
	//Xu ly voi lenh hoi:
	if(n_frame_cmd_id == 0xB1) //lenh tu server : 0xB1 : ON, OFF trang thai
  {
    memcpy(noroute_packet, packet, packet[0]+3);  // Bao gom RSSI+LQI
    if(n_frame_net_id == m_cfg.net_id) // chi xu ly frame cung mang
    {
      if(n_frame_group_id == m_cfg.group_id && m_cfg.group_id > 0)
      {
        if(n_frame_data[0] <= STATE_ALARM && n_frame_data[0] >= STATE_NORMAL)
          led_state = n_frame_data[0];
      }
      
      if(n_frame_level < m_cfg.maxLever ) // chi xu ly chuyen tiep cho node cung mang
      {
        NoRouteTransfer(); //Chuan bi du lieu chuyen tiep
        //Tính toán thời gian tránh xung đột:
        //Nếu do master gọi trực tiếp không dùng NetID thì trả lời ngay lập tức.
        //Nếu có dùng NetID thì chờ cho các Site hết thời gian chuyển tiếp lệnh.
        //Serial.printf("\r\nNoroute transfer [TX][%s][%u]\r\n",S2SH(buff, &packet[1], packet[0], 0, NULL));
        if (n_frame_net_id > 0) //Có yêu cầu mạng
          waitCCA = m_cfg.next_timeout + ((millis() % noroute_packet[0]) + 1) * m_cfg.next_random;
          //waitCCA = m_cfg.next_execute;
        else
          waitCCA = 0;
        //Serial.printf("\r\n***Repair Response: waitCCA=[%u]\r\n", waitCCA);
        waitCCA += millis();
      }
    }
  }
  else if(n_frame_cmd_id == 0x4D) //lenh tra loi tu dong 0x4D tu cac slave
  {
    ////////////Serial.printf("\r\nNoroute Frame: mesh_cmd_index=[%02X] Index=[%02X] Level=[%02X]\r\n", mesh_cmd_index, n_frame_index, n_frame_level); //For debug
    if(n_frame_serial == m_cfg.serial) 
    {
        //chua lm gi
    }
    else
    {
      memcpy(noroute_packet, packet, packet[0]+3);  // Bao gom RSSI+LQI
      if(n_frame_net_id == m_cfg.net_id && m_cfg.net_id > 0) // chi xu ly chuyen tiep cho node cung mang
      {
        if(n_frame_group_id == m_cfg.group_id && m_cfg.group_id > 0)
        {
          
          appendOrUpdate(&head,n_nodeName,(int)n_frame_data[0]);
          //Serial.println((int)n_frame_data[0]);
          printList(head);
          if(n_frame_data[0] == STATE_ALARM && led_state == STATE_NORMAL)      // trong group co node dang alarm
          {
            led_state = STATE_ALARM_LINK;
            //Serial.println("Vao1");
          }
          char sBuff[100];
          if(checkFire(head)==true)
          {
            Serial.println("Vao");
            led_state=STATE_ALARM_LINK;
          }
          if(checkFire(head)==false&&led_state==STATE_ALARM_LINK)
          {
            led_state=STATE_NORMAL;
            if(led_state!=STATE_ALARM_LINK )
            {
              display.clear();
              display.setFont(ArialMT_Plain_16);
              display.setTextAlignment(TEXT_ALIGN_CENTER);
              display.drawString(64, 1, String(m_cfg.nodeName));
              sprintf(sBuff, "ID=%08X", m_cfg.serial);  //Số chế tạo lưu dạng HEXA TEXT
              display.drawString(64, 18, sBuff);
              
              sprintf(sBuff, "Net=%04X-Grp=%02X", m_cfg.net_id,m_cfg.group_id);  //
              display.drawString(64, 34, sBuff);

              display.setFont(ArialMT_Plain_10);
              //sprintf(sBuff, "* %s %s *", __DATE__, __TIME__);
              sprintf(sBuff, "* %s *", WiFi.localIP().toString().c_str());
              display.drawString(64, 50, sBuff);
              display.drawRect(0, 0, 128, 64);
              display.setTextAlignment(TEXT_ALIGN_LEFT);
              display.display();
              led_state_last=led_state;
              
            }
          }
        }

        if(n_frame_level < m_cfg.maxLever ) // chi xu ly chuyen tiep cho node cung mang
        {
          NoRouteTransfer(); //Chuan bi du lieu chuyen tiep
          //Tính toán thời gian tránh xung đột:
          //Nếu do master gọi trực tiếp không dùng NetID thì trả lời ngay lập tức.
          //Nếu có dùng NetID thì chờ cho các Site hết thời gian chuyển tiếp lệnh.
          //Serial.printf("\r\nNoroute transfer [TX][%s][%u]\r\n",S2SH(buff, &packet[1], packet[0], 0, NULL));
          if (n_frame_net_id > 0) //Có yêu cầu mạng
            waitCCA = m_cfg.next_timeout + ((millis() % noroute_packet[0]) + 1) * m_cfg.next_random;
            //waitCCA = m_cfg.next_execute;
          else
            waitCCA = 0;
          //Serial.printf("\r\n***Repair Response: waitCCA=[%u]\r\n", waitCCA);
          waitCCA += millis();
        }
      }
    }
  }
	//Khong thuoc cau truc lenh thi khong xu ly gi.
	return 1;
}

/*CAC HAM SU TRUYEN THONG RF*/
void RF_Mesh_Process() { //Duoc goi sau moi nhip cua TIM2 (~2ms)
	static char RX_Buffer[RF_PACKET_LENGTH];
  if (LoRa_receiveMessage(RX_Buffer)) { //Co frame chua duoc xu ly
      //Serial.printf("\r\nCo frame chua duoc xu ly\r\n");
    if (NorouteCheckFrameOK(RX_Buffer)) 
		  NorouteProcess(RX_Buffer);
    else Serial.printf("\r\n***Noroute frame false!\r\n");
  }
	////////////////////////
	else if (mesh_process) {   
    if (waitCCA < millis()) {
		  if (mesh_process == WAIT_SEND_RESPONSE || mesh_process == WAIT_SEND_TRANSFER) {
        LoRa_sendMessage(&noroute_packet[MFRA_HEADER], noroute_packet[MFRA_LEN]);        
		    mesh_process = WAIT_NOTHING; //Huy qua trinh lenh
		  }
      else if (mesh_process == WAIT_READ_METER)
      {

      }
      else {
        mesh_process = WAIT_NOTHING;
      }
    }
	}
}

// Hàm để tạo một node mới
Node* createNode(char *name, int data) 
{
  Node* newNode = (Node*)malloc(sizeof(Node));
  strcpy(newNode->Name, name);
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}
void appendOrUpdate(Node** head_ref, char *name, int data) 
{
  if (*head_ref == NULL) 
  {
    *head_ref = createNode(name, data);
    return;
  }
  Node* current = *head_ref;
  while (current != NULL) 
  {
    if (strcmp(current->Name, name) == 0) 
    {
      // Node đã tồn tại, cập nhật giá trị data
      current->data = data;
      return;
    }
    if (current->next == NULL) 
    {
      // Đã duyệt đến cuối danh sách, thêm node mới
      current->next = createNode(name, data);
      return;
    }
    current = current->next;
  }
}
bool checkFire(Node* head) 
{
  bool val=false;
  Node* current = head;
  while (current != NULL) 
  {
    if (current->data == 1) 
    {
      val=true;
    }
    current = current->next;
  }
  return val;
}
void printList(Node* node) {
    while (node != NULL) {
        Serial.printf("Name: %s, Data: %d\n", node->Name, node->data);
        node = node->next;
    }
}
extern Node* head;