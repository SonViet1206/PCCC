#include "main.h"
Node* head=NULL;
const char BUILT_DATE_TIME[] = __DATE__ " "__TIME__
                                        "\0";  //__TARGET__ __REVISION__
const char *MONTHS[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

SSD1306Wire  display(0x3c, 500000, OLED_SDA_PIN, OLED_SCL_PIN, GEOMETRY_128_64, OLED_RST_PIN); // addr , freq , i2c group , resolution , rst
MyEEPROM eeprom(1024);     //Dùng EEPROM nội kích thước 1024 bytes
EEPROM_CONFIG_TYPE m_cfg;  //Bien luu thong tin doc tu FLASH

SX1262 LoRa = new Module(LORA_CS_PIN, LORA_DI0_PIN, LORA_RST_PIN, LORA_BUSY_PIN);

MyButton  Btn_In_Red(BTN_IN_RED_PIN,true,20,2000,3000);
MyButton  Btn_In_Green(BTN_IN_GREEN_PIN,true,20,2000,3000);

uint32_t ms_tick_count = 0;  //So giay mili giay dem tang dan sau 49 ngay se tran gia tri
int new_counter = 1234;
int led_state = 0;  //Biến này sẽ luu trữ trạng thái hiên tại của bộ điều khiển
int led_state_last = 0;
int count_send=0;
int rfrx_index = 0;
int rfrx_rssi = 0;
int rfrx_snr = 0;
int rfrx_len = 0;

unsigned long loraRXtimeout = 0;
//Dùng mảng để chứa dữ liệu vì dữ liệu có thể nhận ký tự bất kỳ, kể cả NULL
//Payload của RF Lora có độ dài tối đa là 64 byte
//Bộ đệm frame, byte đầu tiên chứa độ dài của dữ liệu, 2 byte tiếp theo dữ liệu là RSSI và SNR
char RX_Buffer[64 + 4];

const char* host = "esp32";
const char* ssid = "UPDATE FIRMWARE";
const char* password = "12345678";

TaskHandle_t Task_OTA;

WebServer server(80);


const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

const char* serverIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update'>"
    "<input type='submit' value='Update'>"
  "</form>"
  "<div id='prg'>progress: 0%</div>"
  "<br>"
  "<br>"
  "<form name='configForm'>"
    "<label for='nodeName'>Node Name:</label>"
    "<input type='text' id='nodeName' name='nodeName' maxlength='13' placeholder='Enter Node Name'>"
    "<br>"
    "<label for='netId'>Network ID:</label>"
    "<input type='text' id='netId' name='netId' maxlength='4' placeholder='Enter Network ID'>"
    "<br>"
    "<label for='groupId'>Group ID:</label>"
    "<input type='text' id='groupId' name='groupId' maxlength='1' placeholder='Enter Group ID'>"
    "<br>"
    "<label for='timeOK'>Time OK:</label>"
    "<input type='text' id='timeOK' name='timeOK' placeholder='Enter Time OK (seconds)'>"
    "<br>"
    "<label for='timeALARM'>Time ALARM:</label>"
    "<input type='text' id='timeALARM' name='timeALARM' placeholder='Enter Time ALARM (seconds)'>"
    "<br>"
    "<label for='maxLever'>Lever message:</label>"
    "<input type='text' id='maxLever' name='maxLever' placeholder='Enter Lever message '>"
    "<br>"
    "<button onclick='sendConfig()'>Set Config</button>"
  "</form>"
  "<script>"
    "$.ajax({"
      "url: '/getconfig',"
      "type: 'GET',"
      "success: function(response) {"
        "var config = JSON.parse(response);"
        "$('#nodeName').val(config.nodeName);"
        "$('#netId').val(config.net_id);"
        "$('#groupId').val(config.group_id);"
        "$('#timeOK').val(config.timeOK);"
        "$('#timeALARM').val(config.timeALARM);"
        "$('#maxLever').val(config.maxLever);"
      "}"
    "});"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $(this);"
    "var data = new FormData(form[0]);"
    " $.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!');"
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "function sendConfig() {"
    "var nodeName = $('#nodeName').val();"
    "var netId = $('#netId').val();"
    "var groupId = $('#groupId').val();"
    "var timeOK = $('#timeOK').val();"
    "var timeALARM = $('#timeALARM').val();"
    "var maxLever = $('#maxLever').val();"
    "$.ajax({"
    "url: '/setconfig?nodeName=' + nodeName + '&netId=' + netId + '&groupId=' + groupId + '&timeOK=' + timeOK + '&timeALARM=' + timeALARM + '&maxLever=' + maxLever,"
    "type: 'GET',"
    "success: function(response) {"
    "console.log('Config set successfully');"
    "}"
    "});"
    "}"
  "</script>";

void Ini_OTA() 
{
  //Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(SSID_OTA, PASSWORD_OTA);
  Serial.println("");

  // Wait for connection
  uint32_t time=millis();
  while (WiFi.status() != WL_CONNECTED&&millis()-time<5000) 
  {
    WiFi.begin(SSID_OTA, PASSWORD_OTA);
    delay(2500);
    Serial.print(".");
    //time=millis();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  //cấu hình 
  server.on("/setconfig", HTTP_GET, []() {
    if (server.hasArg("netId") && server.hasArg("groupId")) {

      int16_t newNetId = strtol(server.arg("netId").c_str(), NULL, 16);
      int8_t newGroupId = strtol(server.arg("groupId").c_str(), NULL, 16);
      uint16_t newTimeOK = server.arg("timeOK").toInt();
      uint16_t newTimeALARM = server.arg("timeALARM").toInt();
      int8_t newMaxLever = strtol(server.arg("maxLever").c_str(), NULL, 10);
      String newNodeName = server.arg("nodeName");
      if (newNetId >= 0 && newNetId < 65536 && newGroupId >= 0 && newGroupId < 256&&newTimeOK>=0&&newTimeALARM>=0&& newMaxLever>=1) {
        // ID mạng và ID nhóm hợp lệ
        // Cập nhật cấu hình và ghi vào EEPROM nếu cần
        char sBuff[50];
        m_cfg.net_id = newNetId;
        m_cfg.group_id = newGroupId;
        m_cfg.time_OK = newTimeOK;
        m_cfg.time_ALARM = newTimeALARM;
        m_cfg.maxLever=newMaxLever;
        newNodeName.toCharArray(m_cfg.nodeName, newNodeName.length() + 1);
        EEPROM_WriteConfig();
        delay(100);
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 1, "CONTROL BOX");
        sprintf(sBuff, "ID=%08X", m_cfg.serial);  //Số chế tạo lưu dạng HEXA TEXT
        display.drawString(64, 18, sBuff);
        
        sprintf(sBuff, "Net=%04X-Grp=%02X", m_cfg.net_id,m_cfg.group_id);//
        display.drawString(64, 34, sBuff);
        display.setFont(ArialMT_Plain_10);
        //sprintf(sBuff, "* %s %s *", __DATE__, __TIME__);
        sprintf(sBuff, "* %s *", WiFi.localIP().toString().c_str());
        display.drawString(64, 50, sBuff);
        display.drawRect(0, 0, 128, 64);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.display();
        // Phản hồi với thông báo "OK" nếu thành công
        server.send(200, "text/plain", "OK");
      } else {
        // ID mạng hoặc ID nhóm không hợp lệ
        // Phản hồi với thông báo "ERROR"
        server.send(200, "text/plain", "ERROR");
      }
    } else {
      // Thiếu tham số
      server.send(200, "text/plain", "ERROR");
    }
  });
  server.on("/loadConfig", HTTP_GET, []() 
  {
  // Tạo một JSON object chứa thông số cài đặt
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["net_id"] = m_cfg.net_id;
  jsonDoc["group_id"] = m_cfg.group_id;
  jsonDoc["timeOK"] = m_cfg.time_OK;
  jsonDoc["timeALARM"] = m_cfg.time_ALARM;
  jsonDoc["maxLever"]=m_cfg.maxLever;
  // Chuyển đổi JSON thành chuỗi
  String jsonResponse;
  serializeJson(jsonDoc, jsonResponse);

  // Trả về chuỗi JSON
  server.send(200, "application/json", jsonResponse);
  });

 
  server.begin();
  
}

void Process_OTA(void * parameter) 
{
  char sBuff[100];
  bool stt_wifi_before=false;
  bool stt_wifi=false;
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 1,String(m_cfg.nodeName) );
  sprintf(sBuff, "ID=%08X", m_cfg.serial);  //Số chế tạo lưu dạng HEXA TEXT
  display.drawString(64, 18, sBuff);
  
  sprintf(sBuff, "Net=%04X-Grp=%02X", m_cfg.net_id,m_cfg.group_id);  //
  display.drawString(64, 34, sBuff);
  display.setFont(ArialMT_Plain_10);
  //sprintf(sBuff, "* %s %s *", __DATE__, __TIME__);
  sprintf(sBuff, "* %s *", "wifi disonnect ");
  display.drawString(64, 50, sBuff);
  display.drawRect(0, 0, 128, 64);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.display();
  uint32_t timerWifi=millis();
  while(true)
  {
    
    Btn_In_Red.Button_loop();
    Btn_In_Green.Button_loop();
    #ifdef CONTROL_PROCESS
      
      
      if(Btn_In_Red.SortPress())
      {
        led_state = STATE_ALARM;
        count_send=0;
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
          
          
        }
        
        
      } 
      if(Btn_In_Green.SortPress())
      {
        led_state = STATE_NORMAL;
        count_send=0;
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
          
          
        }
      } 
      Update_LED_State();
      if(led_state==STATE_ALARM_LINK&&led_state_last!=STATE_ALARM_LINK)
      {
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 1,"CHAY CTY:" );
        
        display.drawString(64, 18, String(n_nodeName));
        
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
      
      
    
    #endif
    if(WiFi.status() != WL_CONNECTED&&(millis()-timerWifi)>=3000)
    {
      
      WiFi.begin(SSID_OTA, PASSWORD_OTA);
      stt_wifi=false;
      if(stt_wifi_before!=false)
      {
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(64, 1, String(m_cfg.nodeName));
        sprintf(sBuff, "ID=%08X", m_cfg.serial);  //Số chế tạo lưu dạng HEXA TEXT
        display.drawString(64, 18, sBuff);
        
        sprintf(sBuff, "Net=%04X-Grp=%02X", m_cfg.net_id,m_cfg.group_id); //
        display.drawString(64, 34, sBuff);

        display.setFont(ArialMT_Plain_10);
        //sprintf(sBuff, "* %s %s *", __DATE__, __TIME__);
        sprintf(sBuff, "* %s *", "wifi disonnect ");
        display.drawString(64, 50, sBuff);
        display.drawRect(0, 0, 128, 64);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.display();
      }
      timerWifi=millis();
      
    }
    else if(WiFi.status() == WL_CONNECTED)
    {
      
      server.handleClient();
      if(stt_wifi_before==false)
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
      }
      
      stt_wifi=true;
      
    }
    stt_wifi_before=stt_wifi;
    
     
    

  }
  
}
//=======================
void setup() {
  char sBuff[128];
  Serial.begin(115200);
  Serial.println("\r\n\r\n***Starting...");
  //////////////////////////////////////
  Serial.print("\r\nCommon IO Config ... ");

  pinMode(BTN_IN_RED_PIN, INPUT_PULLUP);   //RED Button
  pinMode(BTN_IN_GREEN_PIN, INPUT_PULLUP);  //GREEN Button

  pinMode(BUILDIN_LED_PIN, OUTPUT);
  pinMode(LED_OUT_RED_PIN, OUTPUT);
  pinMode(LED_OUT_GREEN_PIN, OUTPUT);
  pinMode(BUZ_OUT_PIN, OUTPUT);

  digitalWrite(BUILDIN_LED_PIN,LOW);
  digitalWrite(LED_OUT_RED_PIN,HIGH);
  digitalWrite(LED_OUT_GREEN_PIN,HIGH);
  digitalWrite(BUZ_OUT_PIN, HIGH);
  Serial.print("Done!");
  //////////////////////////////////////
  Serial.print("\r\nOLED Setting ... ");
  pinMode(OLED_RST_PIN, OUTPUT);
  digitalWrite(OLED_RST_PIN, LOW);  // set low to reset OLED
  delay(50);
  digitalWrite(OLED_RST_PIN, HIGH);
  // Initialising the UI will init the display too.
  display.init();
  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "***Starting...");
  display.display();
  Serial.print("Done!");
  //////////////////////////////////////
  Serial.print("\r\nLORA Setting ... ");
  display.drawString(0, 10, "LORA Setting ... ");
  display.display();
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_CS_PIN);
  int state = LoRa.begin(868);
  
  if (state == RADIOLIB_ERR_NONE) //Initialize radio
  { 
    
    LoRa.setBandwidth(500E3);
    LoRa.setSpreadingFactor(7);
    LoRa.setOutputPower(22);
    Serial.println("Initialize radio OK.");
  }
  else
  {
    Serial.println("Initialize radio NG.");
  }

  
  //////////////////////////////////////
  #ifdef MASTER_PROCESS
    //Màn hình chào mừng của MASTER

    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 0, "RF MASTER");

    //sprintf(sBuff, "ID=%08X", m_cfg.serial); //Số chế tạo lưu dạng HEXA TEXT
    display.drawString(64, 17, "ANDON SYSTEM");
    //sprintf(sBuff, "Network=%04X", m_cfg.net_id); //
    display.drawString(64, 32, "pmttau.com.vn");

    display.setFont(ArialMT_Plain_10);
    sprintf(sBuff, "* %s *", BUILT_DATE_TIME);
    display.drawString(64, 52, sBuff);

    //display.drawRect(0,0,128,64);
    //display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.display();
    delay(10000);
  #else
    Serial.print("\r\nEEPROM Setting ... ");
    display.drawString(0, 20, "EEPROM Setting ... ");
    display.display();
    eeprom.begin();
    EEPROM_ReadConfig();

    //debug
    //m_cfg.net_id = 0x0101;
    //m_cfg.group_id = 0x01;

    delay(1000);
    //Màn hình chào mừng của SLAVE
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 1, "CONTROL BOX");
    sprintf(sBuff, "ID=%08X", m_cfg.serial);  //Số chế tạo lưu dạng HEXA TEXT
    display.drawString(64, 18, sBuff);
    
    sprintf(sBuff, "Net=%04X-Grp=%02X", m_cfg.net_id,m_cfg.group_id); //
    display.drawString(64, 34, sBuff);

    display.setFont(ArialMT_Plain_10);
    //sprintf(sBuff, "* %s %s *", __DATE__, __TIME__);
    sprintf(sBuff, "* %s *", WiFi.localIP().toString().c_str());
    display.drawString(64, 50, sBuff);

    display.drawRect(0, 0, 128, 64);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.display();
    delay(3000);
  #endif
  Ini_OTA();
  xTaskCreatePinnedToCore(Process_OTA,"Task_OTA",10000,NULL,0,&Task_OTA,0);
  for(int i = 0;i<3;i++)
  {

    Send_Update_State(led_state);
    delay(500);
  }
}

void loop() {
  #ifdef MASTER_PROCESS
    //Kiểm tra nhận frame LORA không dùng ngắt:
    RF_Master_Process();
  #else 
    #ifdef CONTROL_PROCESS
      // Btn_In_Red.Button_loop();
      // Btn_In_Green.Button_loop();
      
      // if(Btn_In_Red.SortPress())
      // {
      //   led_state = STATE_ALARM;
      //   count_send=0;
        
      // } 
      // if(Btn_In_Green.SortPress())
      // {
      //   led_state = STATE_NORMAL;
      //   count_send=0;
      // } 
      
      if(led_state == STATE_ALARM)
      {
        if (runEveryMs(m_cfg.time_ALARM))
        {
          Send_Update_State(led_state);
          
        }
      }
      else{
        if (runEveryMs(m_cfg.time_ALARM))
        {
          if(count_send<20)
          {
            count_send++;
            Send_Update_State(led_state);
          }
        }
      }
      RF_Mesh_Process();
      
      
      

      
    #endif
  #endif
  //Xử lý luông truyền thông công UART:
  UART_Command_Process();
}

void Update_LED_State(void)
{

  
  if(led_state == STATE_ALARM)
  {
    digitalWrite(BUILDIN_LED_PIN,HIGH);
    digitalWrite(LED_OUT_RED_PIN,LOW);
    digitalWrite(LED_OUT_GREEN_PIN,HIGH);
    digitalWrite(BUZ_OUT_PIN,LOW);
  }
  else if(led_state == STATE_NORMAL)
  {
    digitalWrite(BUILDIN_LED_PIN,LOW);
    digitalWrite(LED_OUT_RED_PIN,HIGH);
    digitalWrite(LED_OUT_GREEN_PIN,LOW);
    digitalWrite(BUZ_OUT_PIN,HIGH);
  }
  else if(led_state == STATE_ALARM_LINK)
  {
    if (runEveryMs2(1000))
    {
      digitalWrite(BUILDIN_LED_PIN, !digitalRead(BUILDIN_LED_PIN));
    }   
    digitalWrite(LED_OUT_RED_PIN,LOW);
    digitalWrite(LED_OUT_GREEN_PIN,HIGH);
    digitalWrite(BUZ_OUT_PIN,LOW);
  }
}

boolean runEveryMs2(unsigned long interval) {
  static unsigned long previousMillis2 = 0;
  unsigned long currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 >= interval) {
    previousMillis2 = currentMillis2;
    return true;
  }
  return false;
}
boolean runEveryMs(unsigned long interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

//     1   2 3  4  5    7        11       15 16 17 18       22   
//[RX][24 00 00 4D 0000 84B6FE7E 00000000 00 00 01 00000000 28][22][1000ms][-81dBm][22]
/////////////////////////////////////////
//Cập nhật trạng thái nút bấm với server:
void Send_Update_State(int state) {
  char TX_Buffer[64 + 3];
  sprintf(TX_Buffer, "\r\n***Send LED State = %d", state);
  Serial.printf("%s", TX_Buffer);
  //NoRouteUpdateStateCounter(state);
  //Chuan bi thong tin tra loi lenh:
  TX_Buffer[0] = MFRA_DATA + 2+14;  // byte dữ liệu
  TX_Buffer[1] = 0x24;
  U2S16(&TX_Buffer[MFRA_NETWORK], m_cfg.net_id, 0, 0);      //Net ID tra loi
  TX_Buffer[MFRA_CMDID] = 0x4D;                             //Lệnh trả lời chủ động
  TX_Buffer[MFRA_KEY_INDEX] = n_frame_key | n_frame_index;  //Key nhan lenh
  TX_Buffer[MFRA_LEVEL] = 0x00;                             //Reset thanh tang dau tien
  U2S32(&TX_Buffer[MFRA_SERIAL], m_cfg.serial, 0, 0);       //ID trả lời
  U2S32(&TX_Buffer[MFRA_REQ_ID], 0, 0, 0);                  //ID hỏi
  TX_Buffer[MFRA_RSSI] = n_frame_rssi;                      //RSSI
  TX_Buffer[MFRA_LQI] = n_frame_lqi;                        //LQI
  TX_Buffer[MFRA_DATA] = (uint8_t)state;
  TX_Buffer[MFRA_DATA+1] = (uint8_t)m_cfg.group_id;
  memcpy(&TX_Buffer[MFRA_DATA + 2],m_cfg.nodeName,14);
  FCSADD_Add(&TX_Buffer[1], TX_Buffer[0]);             //Tinh toan FCS
 
  LoRa_sendMessage(&TX_Buffer[1], TX_Buffer[0]);
  
}


//Mẫu hàm xử lý lệnh:
char Command_Process(Stream &_stream, char *sour, char len);

//Hàm này xử lý các frame lệnh kết thúc bằng khoảng lặng timeout
void UART_Command_Process() {
#define RX_MAX_LENGH 64                     //Độ dài tối đa của frame
  static char rx_buffer[RX_MAX_LENGH + 3];  //Bộ đệm nhận frame thêm 3 byte <CR><LF><NULL>
  static int rx_index = 0;                  //Con trỏ vị trí byte đang nhận hay số byte đã nhận
  static unsigned long ms_timeout = 0;

  while (Serial.available())  // get the new byte:
  {
    rx_buffer[rx_index + 1] = (char)Serial.read();
    if (++rx_index >= RX_MAX_LENGH) rx_index = 0;  //Hủy frame vượt độ dài
    ms_timeout = millis();                         //Cập nhật giá trị timeout
  }                                                //End of while (Serial.available())

  if (rx_index > 0)  //Đã có ký tự được nhận
  {
    if (millis() - ms_timeout > 50)  //Thời gian dãn cách byte cuối timeout
    {
      rx_buffer[rx_index + 1] = NULL;  //Đánh dấu kết thúc chuỗi
      rx_buffer[0] = rx_index;
      rx_index = 0;

      if (AT_Command_Process(Serial, &rx_buffer[1], rx_buffer[0])) {  //Hàm xử lý nội dung lệnh

      } 
      else //if (NorouteCheckFrameOK(rx_buffer))
      {
        Serial.printf("\r\n Check Frame OK\r\n");
        //Xu ly Index:
        noroute_last_key_index = (n_frame_key & 0xF0) | ((noroute_last_key_index + 1) & 0x0F);
        rx_buffer[MFRA_KEY_INDEX] = noroute_last_key_index;
        rx_buffer[MFRA_LEVEL] = 0;  //Xuat phat tu master

        FCSADD_Add(&rx_buffer[1], rx_buffer[0]);        //Tinh toan FCS:
        LoRa_sendMessage(&rx_buffer[1], rx_buffer[0]);  //Chuyen tiep lenh
        //}
      }
    }
  }
}


void RF_Master_Process() {
#define RX_MAX_LENGH 64                     //Độ dài tối đa của frame
  static char rx_buffer[RX_MAX_LENGH + 3];  //Bộ đệm nhận frame thêm 3 byte <CR><LF><NULL>
  static int rx_index = 0;                  //Con trỏ vị trí byte đang nhận hay số byte đã nhận

  if (LoRa_receiveMessage(RX_Buffer)) {

  }
}

int LoRa_receiveMessage(char *rxBuffer) {
  byte byteArr[64];
  int state = LoRa.receive(byteArr, 64);
  
  if (state == RADIOLIB_ERR_NONE) 
  {
    // packet was successfully received
    int packetSize = LoRa.getPacketLength(true);
    
    if(packetSize>=1 && packetSize <= 64 )
    {
      
      for (int i = 0; i < packetSize; i++) rxBuffer[i+1] = (char) byteArr[i];
      char buff[RF_PACKET_LENGTH * 3];          //Du kich thuoc chua chuoi dang HEXA + cac byte thong tin
      uint8_t p = 0;
      uint32_t ms_tick = millis() - ms_tick_count;
      ms_tick_count = millis();

      ++rfrx_index;  //Biến đếm frame nhận được
      rfrx_len = packetSize;
      rfrx_rssi = LoRa.getRSSI();           //(int)RX_Buffer[rfrx_len + 1] - 256;
      rfrx_snr = (int)(LoRa.getSNR() * 4);  //(int)RX_Buffer[rfrx_len + 2];

      rxBuffer[0] = (char)packetSize;
      rxBuffer[packetSize + 1] = (char)rfrx_rssi;  //LoRa.packetRssi();
      rxBuffer[packetSize + 2] = (char)rfrx_snr;   //(LoRa.packetSnr() * 4);
      
      if (ms_tick > 900)
        Serial.printf("\r\n[RX][%s][%u][%ums][%ddBm][%u]\r\n", S2SH(buff, &rxBuffer[1], rfrx_len, 0, NULL), rfrx_len, ms_tick, rfrx_rssi, rfrx_snr);
      else
        Serial.printf("[RX][%s][%u][%ums][%ddBm][%u]\r\n", S2SH(buff, &rxBuffer[1], rfrx_len, 0, NULL), rfrx_len, ms_tick, rfrx_rssi, rfrx_snr);

    } 
    else rxBuffer[0] = 0;
    return 1;
  }
  return 0;
}


int LoRa_sendMessage(char *txBuffer, size_t tx_size) {
  char buff[RF_PACKET_LENGTH * 3];  //Du kich thuoc chua chuoi dang HEXA + cac byte thong tin
  uint32_t ms_elapse;
  uint32_t ms_tick = millis() - ms_tick_count;  //Thời gian từ lần truyền frame trước
  ms_tick_count = millis();

  if(LoRa.transmit((byte*)txBuffer, tx_size)== RADIOLIB_ERR_NONE)
  {
    ms_elapse = millis() - ms_tick_count;  //Thời gian truyền hết frame
    if (ms_tick > 900)
      Serial.printf("\r\n[TX][%s][%u][%ums][%ums]\r\n", S2SH(buff, txBuffer, tx_size, 0, NULL), tx_size, ms_tick, ms_elapse);
    else
      Serial.printf("[TX][%s][%u][%ums][%ums]\r\n", S2SH(buff, txBuffer, tx_size, 0, NULL), tx_size, ms_tick, ms_elapse);
  }
  return 1;
}