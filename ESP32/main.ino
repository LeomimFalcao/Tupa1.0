/*

ESTAÇÃO METEOROLÓGICA TUPÃ 1.0[Temperatura - Umidade - Pressão Atmosférica]
Grupo: Daniel Sarmento, Iury Matheus, Leomim Falcão, Victor Herbert

GNU General Public License
Escrito em: 30 Jan 2019.
Ùltima atualização: 15 Fev 2019.

-------------------------------------------------
                 Relação de Pinos
-------------------------------------------------

Vin (Voltage In) -> 3.3V
Gnd (Ground) -> Gnd
SDA (Serial Data) -> Pino D21 no NODEMCU ESP32
SCK (Serial Clock) -> Pino D22 no NODEMCU ESP32
 
-------------------------------------------------
*/

#include <BME280I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SERIAL_BAUD 115200

#define BUZZER_PIN 12
#define BUZZER_FREQ 2000
#define BUZZER_CHANNEL 0
#define BUZZER_RESOLUTION  8

bool qtBuzzer = false, siteBuzzer = false;

String siteRequest,qtRequest;
//const char* ssid = "Tupa1";
//const char* password =  "patonaodorme";
const char* ssid = "Gothan_GWR-110";
const char* password =  "23021968";

BME280I2C bme; // Objeto que modela o sensor

BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);  

float temp, pres, umi;
String s_temp, s_pres, s_umi;

void updateReadings()// Solicita novas medições ao sensor e salva nos formatos para envio
{
    bme.read(pres, temp, umi, tempUnit, presUnit);
    pres /= 101325;
    s_pres = String(pres);
    s_temp = String(temp);
    s_umi = String(umi);
}

void sendSite()
{
    if(WiFi.status()== WL_CONNECTED)//Verifica o Status do Wi-Fi
    {       

        HTTPClient http;
        http.begin("http://tupa1.herokuapp.com/upload");//Especifica o destino para o pedido HTTP
        http.addHeader("Content-Type", "application/json");//Especifica o tipo de conteúdo do header       
        
        siteRequest =
            "{\"temp\":" +s_temp +
            ",\"umi\":"+ s_umi +
            ",\"pre\":"+ s_pres+"}";

        int httpResponseCode = http.POST(siteRequest);//Envia as informações

        if(httpResponseCode < 1){
            Serial.print("[ERROR] Code: ");
            Serial.println(httpResponseCode);
        }

        http.end();  //Libera processos
    }

}

void readSite()
{

    if (WiFi.status() == WL_CONNECTED){  //Verifica a situação atual da conexão 
        HTTPClient http;
        http.begin("http://tupa1.herokuapp.com/download");//Especifica o destino para o pedido HTTP 
        int httpCode = http.GET();// Realiza o Pedido
    
        if (httpCode > 0){ //Verifica o código de resposta
            String payload = http.getString(); // Guarda as informações recebidas do site
            qtBuzzer = (payload == "1");
        }
        else
            Serial.println("Erro no pedido HTTP");//Se o código de resposta for inválido, mostra na serial o erro no pedido HTTP

        http.end(); //Libera processos.
    }
      
}

void readQT()
{
    if(Serial.available()){
        siteBuzzer = (Serial.read() == 49); // Se a resposta for 49 ,'1' em ascii, ativa o Buzzer
    }//Espera até a serial estar disponível para realizar a leitura   
}

void sendQT()
{
    qtRequest = "{temp:"+ s_temp + "," + "umi:"+ s_umi + "," + "pre:"+ s_pres + "}";// String concatenada para envio ao QT
    Serial.println(qtRequest);
    Serial.flush();
}


void updateBuzzer()
{    
    if(qtBuzzer || siteBuzzer)
        ledcWriteTone(0, 2000);
    else // Se não, mantém desligado, ou desliga-o
        ledcWriteTone(0, 0);
}

void setup()
{
    
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);// Configuração dos Pinos do Buzzer
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    
    //__________________________________________________________________________

    Serial.begin(SERIAL_BAUD); // Inicia a comunicação serial na velocidade estipulada
    
    //________________________________________________________________________

    Wire.begin(); // Necessário para Comunicação I2C do Sensor

    Serial.print("Conectando ao Sensor BME280...");
    while(!bme.begin()) // Espera a comunicação do sensor
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    switch(bme.chipModel()) // Mostra na serial o modelo do sensor utilizado
    {
        case BME280::ChipModel_BME280:
        Serial.println("Conectado");
        break;

        default:
        Serial.println("Sensor Desconhecido Encontrado!");
    }

    
    
    //__________________________________________________________________________
     
    WiFi.begin(ssid, password);// Conecta a rede Wi-Fi desejada

    Serial.print("Conectando ao WiFi...");
    while (WiFi.status() != WL_CONNECTED) //Verifica a conexão
    {
        Serial.print(".");
        delay(500);
        WiFi.begin(ssid, password);
    }
    Serial.println();
    Serial.print("Conectado a ");
    Serial.println(ssid);

    //__________________________________________________________________________

}



void loop()
{
    updateReadings();

    sendQT();
    sendSite();
    
    readQT();
    readSite();

    updateBuzzer();

    delay(5000);
}
//__________________________________________________________________________
