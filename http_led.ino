#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        12 // On Trinket or Gemma, suggest changing this to 1


//define the DS1802b port
#define ONE_WIRE_BUS 14
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 256 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixel
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels


Ticker flipper;
Ticker changetemp;
const char* ssid = "wrt1";
const char* password = "11Mortonstreet";
 
int ledPin = 13; // GPIO13
WiFiServer server(80);
bool flag=false;
bool bobflag = false;
bool closedisplay = false;
bool tempflag = true;
static uint8_t showtempcount = 0;

//const long utcOffsetInSeconds = 39600; //daytime saving
const long utcOffsetInSeconds = 36000; // withoudt daytime saving 

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
// Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
uint32_t frame[8 * 32];
uint32_t BLUE = pixels.Color(0, 0, 255);
uint32_t RED = pixels.Color(255, 0, 255);
uint32_t GREEN = pixels.Color(0, 255, 0);
uint32_t MIX = pixels.Color(0, 255, 255);
uint32_t WHITE = pixels.Color(255, 255, 255);
uint32_t BLACK = pixels.Color(0, 0, 0);



uint64_t DIGITS[10] = {0x007e8181817e,0x000082ff8000,0x0041c1859171,0x004281899976,0x000c282422ff,0x00f28991898e,0x007e8991894e,0x0080e18809e0,0x006e8991896e,0x00729189917e};





void showdigitfillframe(uint32_t frame[], uint32_t startpos, uint64_t digit, int numbers){
   int counter = 0;
   while (true){

       if (counter == numbers)
           break;

        if (digit %2 ==1){
            frame[startpos + counter] = 1;}
        else{frame[startpos + counter] = 0; }
        counter += 1;
        digit = digit >> 1;
    }
}

   void setup()
   {
       Serial.begin(115200);
       delay(10);

       pinMode(ledPin, OUTPUT);
       digitalWrite(ledPin, LOW);

       // Connect to WiFi network
       Serial.println();
       Serial.println();
       Serial.print("Connecting to ");
       Serial.println(ssid);

       WiFi.begin(ssid, password);

       while (WiFi.status() != WL_CONNECTED)
       {
           delay(500);
           Serial.print(".");
       }
       Serial.println("");
       Serial.println("WiFi connected");

       // Start the server
       server.begin();
       Serial.println("Server started");

       // Print the IP address
       Serial.print("Use this URL to connect: ");
       Serial.print("http://");
       Serial.print(WiFi.localIP());
       Serial.println("/");
       //start the temperature sensor
       sensors.begin();


       // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
       // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
    memset(frame, 0, sizeof(uint32_t) * 8 * 32);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(10);
  pixels.clear();
  flipper.attach(2, isrFunc); //tickerObj.attach(timeInSecs,isrFunction)
  changetemp.attach(2, changetempflag);
  timeClient.begin();


}



void addstripdot(uint32_t frame[], int color){
//we add strip at place col 16 and 17
  for (int i=0;i<3*8;i++){
    frame[14 * 8 + i] = color;
  }

//  for (int j=0;j<3;j++){
    int j=0;
    frame[14 * 8 + 8*j+7] = 0;
    frame[14 * 8 + 8*j+6] = 0;
    frame[14 * 8 + 8*j+5] = 0;
    frame[14 * 8 + 8*j+4] = 0;
    frame[14 * 8 + 8*j+3] = 0;
    j=1;
    frame[14 * 8 + 8*j+0] = 0;
    frame[14 * 8 + 8*j+1] = 0;
    frame[14 * 8 + 8*j+2] = 0;
    frame[14 * 8 + 8*j+3] = 0;
    frame[14 * 8 + 8*j+4] = 0;
    j=2;
    frame[14 * 8 + 8*j+7] = 0;
    frame[14 * 8 + 8*j+6] = 0;
    frame[14 * 8 + 8*j+5] = 0;
    frame[14 * 8 + 8*j+4] = 0;
    frame[14 * 8 + 8*j+3] = 0;
    
    


    
//  }
  
}


void addstripframe(uint32_t frame[], int color){
//we add strip at place col 16 and 17
  for (int i=0;i<3*8;i++){
    frame[14 * 8 + i] = color;
  }
}

void transfertostrip(Adafruit_NeoPixel pixels, uint32_t frame[]){
    //frame size is 8*32
    for (uint32_t index = 0; index < 8*32; index++){

        switch (frame[index])
        {
        case 1:
            pixels.setPixelColor(index, RED);
            break;
        case 2:
            pixels.setPixelColor(index, BLUE);
            break;
        case 3:
            pixels.setPixelColor(index, GREEN);
            break;
        default:
            pixels.setPixelColor(index, pixels.Color(0, 0, 0));
            break;
        }
    }
}


void changetempflag(){
    
  showtempcount +=1;
  if (showtempcount > 3){
    showtempcount = 0;
    tempflag = true;
  }else{
    tempflag= false;
    }
  

  
  
}

void isrFunc()
{
  timeClient.update();
  int h=0;
  int m=0;
  int clock[4];
  if (tempflag == true){
    sensors.requestTemperatures(); // Send the command to get temperatures
    float tempval = sensors.getTempCByIndex(0);
    int temp100 = int(tempval*100);
    // in melbourne we do not have temp below 0
    clock[3]=temp100/1000;
    clock[2]=(temp100%1000)/100;
    clock[1]=(temp100%100)/10;
    clock[0]=temp100%10;


  }else{
    int h = timeClient.getHours();
    int m = timeClient.getMinutes();
     if (h > 9)
    {
        clock[3] = h / 10;
        clock[2] = h % 10;
    }
    else
    {
        clock[2] = h;
        clock[3] = 0;
    }

    if (m > 9)
    {
        clock[0] = m % 10;
        clock[1] = m / 10;
    }
    else
    {
        clock[1] = 0;
        clock[0] = m;
    }
    
  }
  
   

    if (closedisplay == true){
   
      pixels.clear();
      pixels.show();

      return;
    }

    if (bobflag == true){
      pixels.setBrightness(255);
      for (int i=0;i<256;i++){
           pixels.setPixelColor(i, WHITE);
        }
       pixels.show();
      return;
    }

   

    uint32_t currentpos = 0;
    for (int each = 0; each < 4; each++)
    {
        showdigitfillframe(frame, currentpos, DIGITS[clock[each]], 48);
        if (each == 1)
        {
            currentpos += 80;
        }
        else
        {
            currentpos += 64;
        }
    }

    if (tempflag == true){
      addstripdot(frame,3);
      
     }else{
    
          if (flag ==  true){
                  addstripframe(frame, 2);
                 
                  flag = false;
              }
          else{
              addstripframe(frame, 3);
              flag = true;
          }
     }


 for (uint32_t index = 0; index < 8*32; index++){

        switch (frame[index])
        {
        case 1:
            pixels.setPixelColor(index, RED);
            break;
        case 2:
            pixels.setPixelColor(index, BLUE);
            break;
        case 3:
            pixels.setPixelColor(index, GREEN);
            break;
        default:
            pixels.setPixelColor(index, pixels.Color(0, 0, 0));
            break;
        }
}
  pixels.setBrightness(2);

  pixels.show();

  
}
 
void loop() {
//   ESP.deepSleep(20e9); // 20e6 is 20 microseconds
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
 

 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);    
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=%20on") != -1)  {
//    digitalWrite(ledPin, HIGH);
    bobflag = true;
  }
  if (request.indexOf("/LED=%20off") != -1)  {
//    digitalWrite(ledPin, LOW);
    bobflag = false;
  }

  if (request.indexOf("/DISPLAY=%20on") != -1)  {
    closedisplay = false;
  }
  if (request.indexOf("/DISPLAY=%20off") != -1)  {
    closedisplay = true;
  }

  
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(bobflag == true) {
    client.print("On");
  } else {
    client.print("Off");
  }

  
 client.print("Close Display: ");
  if(closedisplay == true) {
    client.print("Closed");
  } else {
    client.print("On");
  }
  
  
  client.println("<br><br>");
  client.println("<a href=\"/LED=%20on\"\"><button><font size=\"20\" color=\"blue\">Turn On </font> </button></a>");
  client.println("<a href=\"/LED=%20off\"\"><button><font size=\"20\" color=\"black\">Turn Off </font> </button></a><br />");  

  client.println("<br><br>");


  
  client.println("<a href=\"/DISPLAY=%20on\"\"><button><font size=\"20\" color=\"blue\">Turn On </font> </button></a>");
  client.println("<a href=\"/DISPLAY=%20off\"\"><button><font size=\"20\" color=\"black\">Turn Off </font> </button></a><br />");  
  
  client.println("</html>");



 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
