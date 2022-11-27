#include <ArduinoJson.h>
#include <WiFi.h>

const char* ssid = "23561398";  // insert your SSID
const char* password = "nicole1212";  // insert your password
const char* host = "api.openweathermap.org";
const char* resource = "/data/2.5/weather?id=1668341&appid=07d19de245e5e799834d307842db7b2f"; 
char jsonRead[600]; //讀取response後儲存JSON資料的變數，必須是全域變數

const char* host1 = "maker.ifttt.com"; //IFTTT server網址
const char* event = "123";  //IFTTT事件名稱
const char* apiKey = "bwDaQbNM0aaLCJZjrkH8FB";  //IFTTT Applet key

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void loop(){

    //以char*格式儲存回傳的json
    char* json = getdata();
    int size = 600;
    Serial.println(json);
    
    //將字串轉換為JSON，儲存在doc中
    StaticJsonDocument<600> doc;
    deserializeJson(doc, json,size);
    
    const char* weather = doc["weather"][0]["description"];
    double temp = doc["main"]["temp"];
    double value2 = temp-273;
    double value3 = doc["main"]["humidity"];
    
    Serial.print("Type: ");
    Serial.println(weather);
    Serial.print("Temp: ");
    Serial.print(value2 ); 
    Serial.println("C");
    Serial.print("Humidity: ");
    Serial.print(value3);
    Serial.println("%");

    ClientRequest(String(weather), String(value2),String(value3));  //Client傳送資料
  
    
    delay(3000); // the OWM free plan API does NOT allow more then 60 calls per minute
}

char* getdata(){
  
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(host, httpPort)) {

    //Client傳送
    client.println(String("POST ") + resource + " HTTP/1.1");
    client.println(String("Host: ") + host); 
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料  
    int timeout = 0;
    while(!client.available() && (timeout++ <= 50)){
      delay(100);
    }
  
    //如果無回應
    if(!client.available()) {
      Serial.println("No response...");
    }

    //Checking for the end of HTTP stream
    while(!client.find("\r\n\r\n")){
      // wait for finishing header stream reading ...
    }

    //讀取資料並儲存在jsonRead中
    client.readBytes(jsonRead, 600);

    //停止Client
    client.stop(); 

    //回傳
    return jsonRead;
  } 
}
void ClientRequest(String weather, String value2, String value3){
  
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
  String JsonString = "";  //此範例不會用到

  String value1="";

  for(int i=0; i<weather.length(); i++){
       if(weather[i]!=' ') value1+=weather[i];
       else if(weather[i]==' ')value1+="%20";  //將空白轉換成"%20"，為URL中的空白字符
}


  //Client連結Server
  if (client.connect(host1, httpPort)) {
    
    //Webhook API
    String url = "/trigger/" + String(event) + "/with/key/" + String(apiKey);
    //Query String
    url += "?value1=" + value1 + "&value2=" + value2 + "&value3=" + value3;

    //Client傳送
    client.println(String("POST ") + url + " HTTP/1.1");
    client.println(String("Host: ") + host1); 
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料  
    int timeout = 0;
    while(!client.available() && (timeout++ <= 50)){
      delay(100);
    }
  
    //如果無回應
    if(!client.available()) {
      Serial.println("No response...");
    }
    //用while迴圈一字一字讀取Response
    while(client.available()){
      Serial.write(client.read());
    }

    //停止Client
    client.stop(); 
  } 
}
