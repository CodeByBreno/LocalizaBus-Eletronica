#define pinoWifiConnected 5
#define pinoLocation 18

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <sstream>
#include <WiFi.h>
#include <HTTPClient.h>

StaticJsonDocument<200> create_json_gps(StaticJsonDocument<200>);
StaticJsonDocument<200> add_location_to_JSON(StaticJsonDocument<200>);
StaticJsonDocument<200> add_date_to_JSON(StaticJsonDocument<200>);
StaticJsonDocument<200> add_time_to_JSON(StaticJsonDocument<200>);
StaticJsonDocument<200> gpsPutDataInJSON(StaticJsonDocument<200>);

int correctHour(uint8_t);
std::string correctValue(int);

const char* ssid = "BrenoNet";
const char* password = "breno123";

int TIME_DELAY = 500;
int FATOR_FUSO_HORARIO = 3;

int c = 0;
bool flagWifiConnected = false;
bool flagLocationFound = false;
bool firstLocation = false;

double time_to_locate = 0;

//Configurações do GPS
TinyGPSPlus gps;

void setup() {
  // Comunicação com o Computador
  Serial.begin(115200);

  // Comunicação ESP32 e Modulo GPS
  Serial2.begin(9600);//Usa os pinos da Serial 2 do ESP32(RX:16 e TX:17)
  Serial.println("SUCESS: A comunicação do ESP32 com o Computador está ocorrendo normalmente");

  // Coloco os pinos de saída como OUTPUT
  pinMode(pinoWifiConnected, OUTPUT);
  pinMode(pinoLocation, OUTPUT);

  // Zero o valor inicial dos pinos de saída
  digitalWrite(pinoWifiConnected, LOW);
  digitalWrite(pinoLocation, LOW);

  // Conexão WiFi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(TIME_DELAY);
    Serial.print(".");
    systemLED();
  }
  Serial.println("\nSUCESS: Conectado com Sucesso!");
}

void loop() {  
  // Cria um objeto JSON para guardar as informações - com tamanho de 200 Bytes
  StaticJsonDocument<200> gpsDataJSON;

  // Verifica se a Comunicação ESP32 com Modulo GPS está disponível
  while (Serial2.available() > 0) {

    // Lê os dados do GPS ligado à serial
    char gpsData = Serial2.read();
    
    // Cria um objeto JSON com os dados fornecidos pelo GPS 
    if (gps.encode(gpsData)){   
      
      // Produz uma mensagem na serial com o formato adequado
      // Começando com um separados, depois o número da transmissão
      // e então os dados presentes no JSON até então (são enviados
      // dentro do gpsPutDataInJSON)
      Serial.println("--------------------------");
      transmissionNumber();

      gpsDataJSON = gpsPutDataInJSON(gpsDataJSON); 
      if (WiFi.status() == WL_CONNECTED && flagLocationFound)  {  
        connectionSendDataToServer(gpsDataJSON);  
      }

      systemLED();
      delay(TIME_DELAY);
    }
  } 

  flagLocationFound = false;
  systemLED();
  delay(TIME_DELAY);
}

void transmissionNumber(){
  // Apresenta o número da transmissão
  c++;
  std::ostringstream transmission;
  transmission << "Transmissão número " << c; 
  Serial.println(transmission.str().c_str());
}

void systemLED(){
  if (WiFi.status() == WL_CONNECTED) { digitalWrite(pinoWifiConnected, HIGH); }
  else                               { alternateLed(pinoWifiConnected);       }

  if (flagLocationFound)             { digitalWrite(pinoLocation, HIGH);      }
  else                               { alternateLed(pinoLocation);            }
}

void connectionSendDataToServer(StaticJsonDocument<200> gpsDataJSON){
    HTTPClient http;

    http.begin("http://67.205.172.182:3333/coordinates");
    http.addHeader("Content-Type", "application/json");

    String jsonString;
    serializeJson(gpsDataJSON, jsonString);
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0){
      Serial.println("Apresentando HTTP e Resultado: ");
      Serial.println(http.getString());
      Serial.println(httpResponseCode);
    } else{
      Serial.println("ERROR: Pacote em Formato Inadequado (Problema no Envio)");
    }

    http.end();
}

StaticJsonDocument<200> gpsPutDataInJSON(StaticJsonDocument<200> gpsDataJSON){
    gpsDataJSON = create_json_gps(gpsDataJSON);

    // Apresenta o JSON por motivos de DEBUG
    Serial.println("Apresentando JSON: ");
    serializeJsonPretty(gpsDataJSON, Serial);
    Serial.println();

    debugInformTimeToLocation();

    return gpsDataJSON;
}

// -------------------------------------------
// Funções de Debug
// -------------------------------------------

void debugInformTimeToLocation(){
    std::ostringstream oss;

    // Informa o tempo decorrido até conseguir obter os dados de localização
    if ((flagLocationFound == true) && (firstLocation == false)){
      firstLocation == true;
      time_to_locate = c*TIME_DELAY/60000;
    } 
    if (firstLocation == true){
      oss.str("");
      oss << "Tempo até detecção da localização : " << time_to_locate << " minutos";
      Serial.println(oss.str().c_str());
    }
}
    
// -------------------------------------------
// Funções para Capturar a Localização
// -------------------------------------------

StaticJsonDocument<200> create_json_gps(StaticJsonDocument<200> data){
  data.clear();
  data = add_location_to_JSON(data);
  data = add_date_to_JSON(data);
  data = add_time_to_JSON(data);
  return data;
}

StaticJsonDocument<200> add_location_to_JSON(StaticJsonDocument<200> data){
  //Adiciona os dados de localização ao JSON
  if (gps.location.isValid()){
    flagLocationFound = true;

    data["bus_id"] = 5;
    data["lat"] = gps.location.lat();
    data["long"] = gps.location.lng();
    data["velocidade"] = gps.speed.kmph();
  } else {
    flagLocationFound = false;

    Serial.println("ERROR: Dados da Localização não Encontrados");
  }

  return data;
}

StaticJsonDocument<200> add_date_to_JSON(StaticJsonDocument<200> data){
  //Adiciona a data do registro ao JSON
  if (gps.date.isValid()){  
    std::ostringstream oss;
    std::string dia = correctValue(gps.date.day());  //Correct torna "5" em "05"
    std::string mes = correctValue(gps.date.month());
    std::string ano = correctValue(gps.date.year());
    oss << dia << "/" << mes << "/" << ano;
    data["data"] = oss.str();

  } else {
    Serial.println("ERROR: Dados da Data não Encontrados");
  }

  return data;
}

StaticJsonDocument<200> add_time_to_JSON(StaticJsonDocument<200> data){
  //Adiciona a hora do registro ao JSON
  if (gps.time.isValid()){
    std::ostringstream oss;
    std::string hora = correctValue(correctHour(gps.time.hour())); //CorrectValue torna "5" em "05"; CorrectHour torna -2 em 22 (2 horas antes das 24)
    std::string minutos = correctValue(gps.time.minute());
    std::string segundos = correctValue(gps.time.second());
    oss << hora << ":" << minutos << ":" << segundos;
    data["hora"] = oss.str();
  } else {
    Serial.println("ERROR: Dados da Hora não encontrados");
  }

  return data;
}

// -------------------------------------------
// Funções Auxiliares
// -------------------------------------------

//Ajeita o horário, trocando valores negativos pelo equivalente valor antes das 24 horas e ajustando o fuso
int correctHour(uint8_t rawHour){
    int correctHour = (int) rawHour - FATOR_FUSO_HORARIO;
    if (correctHour < 0) { correctHour = 24 + correctHour; }
    return correctHour;
}

//Ajeita a representação em string de um número, retornando-o na forma 0X se seu valor for menor que 10
std::string correctValue(int value){
    std::ostringstream oss;
    if (value < 10) { oss << "0" << value; }
    else { oss << value; }
    return oss.str();
}

void alternateLed(uint8_t pino){
  if (digitalRead(pino) == HIGH){
    digitalWrite(pino, LOW);
  } else{
    digitalWrite(pino, HIGH);
  }
} 