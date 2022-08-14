#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SoftwareSerial.h>


///////////////////////////////////////////////////////////////////////////
///////////////////////////// CONFIGURATION ///////////////////////////////
///////////////////////////////////////////////////////////////////////////


// WIFI CREDENTIALS
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_WIFI_PASSWORD";

// SLACK SETTINGS
const String apiKey = "YOUR_SLACK_HOOK_KEY";
const String serverName = "https://hooks.slack.com/services/" + apiKey;
const String SLACK_INFO_CHANNEL_NAME = "status-plant";

const char* root_ca= \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIGxDCCBaygAwIBAgIQC+eFf+ZWEfrvT9UqTq+HkTANBgkqhkiG9w0BAQsFADBP\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMSkwJwYDVQQDEyBE\n" \
    "aWdpQ2VydCBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTAeFw0yMjAzMjgwMDAwMDBa\n" \
    "Fw0yMzAzMjgyMzU5NTlaMHExCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9y\n" \
    "bmlhMRYwFAYDVQQHEw1TYW4gRnJhbmNpc2NvMSEwHwYDVQQKExhTbGFjayBUZWNo\n" \
    "bm9sb2dpZXMsIEluYy4xEjAQBgNVBAMTCXNsYWNrLmNvbTCCASIwDQYJKoZIhvcN\n" \
    "AQEBBQADggEPADCCAQoCggEBAM7KVB44xpOaAMqktUbSbDm2mVwVXtbY5LGe1sn4\n" \
    "XczOQitonvfbABgkOw4SZI7lHhZoZr5gCg5ezZt/b5A0RTZoFcH3kkdLa4H6JcdA\n" \
    "YlW811Z+9fjfw18rV+u3lzLy8XawSr0mjRfoSj+mr7fOTBl7wuvC87FRq4jbfh8E\n" \
    "7LHsns7h2IG8fOxMrXDbaUpQ9DZwp0pDKYRpm/I2X1IMG0djw9dUHxhQnguqI8Wc\n" \
    "HE8uYBWywiJvV9OQ2Lmr/41jPhmPjSWnq/6Uw1yp12F2hTtbvNwERAbMZPoEUTHJ\n" \
    "Ib3KtrxU9U7qhTbNVDfMDV/Zy+bcm3qKjNKsag1N3L2SluUCAwEAAaOCA3gwggN0\n" \
    "MB8GA1UdIwQYMBaAFLdrouqoqoSMeeq02g+YssWVdrn0MB0GA1UdDgQWBBSo+rQM\n" \
    "T7DghxWhCT4VpRvlxfCCsDAhBgNVHREEGjAYgglzbGFjay5jb22CCyouc2xhY2su\n" \
    "Y29tMA4GA1UdDwEB/wQEAwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUH\n" \
    "AwIwgY8GA1UdHwSBhzCBhDBAoD6gPIY6aHR0cDovL2NybDMuZGlnaWNlcnQuY29t\n" \
    "L0RpZ2lDZXJ0VExTUlNBU0hBMjU2MjAyMENBMS00LmNybDBAoD6gPIY6aHR0cDov\n" \
    "L2NybDQuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0VExTUlNBU0hBMjU2MjAyMENBMS00\n" \
    "LmNybDA+BgNVHSAENzA1MDMGBmeBDAECAjApMCcGCCsGAQUFBwIBFhtodHRwOi8v\n" \
    "d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwfwYIKwYBBQUHAQEEczBxMCQGCCsGAQUFBzAB\n" \
    "hhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wSQYIKwYBBQUHMAKGPWh0dHA6Ly9j\n" \
    "YWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydFRMU1JTQVNIQTI1NjIwMjBDQTEt\n" \
    "MS5jcnQwCQYDVR0TBAIwADCCAYAGCisGAQQB1nkCBAIEggFwBIIBbAFqAHYArfe+\n" \
    "+nz/EMiLnT2cHj4YarRnKV3PsQwkyoWGNOvcgooAAAF/0jpf2gAABAMARzBFAiAc\n" \
    "+VymUk+H/DgMtfKYkUD5DYjeB7Hborp2rYiDJOWCJQIhANlU7w8BBLKSiQQx77Ya\n" \
    "xUzYoX6nZRXLa9Nm/AGTbSoRAHcANc8ZG7+xbFe/D61MbULLu7YnICZR6j/hKu+o\n" \
    "A8M71kwAAAF/0jpfvAAABAMASDBGAiEA6iH+JXN7eNUea3VSaoSNrAZvcoVS4Yne\n" \
    "qv410Ki8lPICIQCY0YfuFaHCTlwPxoG9py6Sg3qFo320/x9QkPxlFx+o/wB3ALNz\n" \
    "dwfhhFD4Y4bWBancEQlKeS2xZwwLh9zwAw55NqWaAAABf9I6X9oAAAQDAEgwRgIh\n" \
    "AOQof1Y9rxImy7XftH7D9GmPOwMNSokOqff7LJBGCG5VAiEAxxZp0l68exntl9Mt\n" \
    "CzDp7yM7MI7tMwrcYOL1OqMxccwwDQYJKoZIhvcNAQELBQADggEBAABAbedNTeeP\n" \
    "UhhleyAQAoy9Rrmzd2CWaSboi1MRZU/Er5RK/XrBd44p9pJNed3XzaU9cKOdSbxR\n" \
    "7phLMnvnVk++uWnncgCy/BfzhD9Uu+8uqmeABXjmwqQyywa/SLKq0xbJ0C5TWI1b\n" \
    "fti/RMscy/2JQlimEI5fWhGBoRcEEf0oJNfkQdqrj1ybtcgdXp135EEgDi4SK6dc\n" \
    "eTbJgeoiW3mBcIdSbFKTxewjzVFcfpjhh29EoaxznKJlkEoGjpgNNezDd+JvSG/o\n" \
    "bMFtFC2g8+V6pb28GJmHlDAycROvaSk9fW2IQOoJj4dTs3v1y99DDq4TefsxNgdb\n" \
    "fQBxn79u8BY=\n" \
    "-----END CERTIFICATE-----";

// Serial Communication Pins for Arduino Watering Controller Connection
#define RXD2 16
#define TXD2 17


// TIMING / NOTIFICATIONS
unsigned long MINS = 1000 * 60;
unsigned long HOURS = MINS * 60;

unsigned long lastInfoTime = 0;

// sends »still alive« message to slack
unsigned long timerAliveDelay = 6 * HOURS;

// TYPES OF SLACK MESSAGES
typedef enum {
    INFO,
    BOOT,
    WARNING, 
    DANGER
} MessageType;


void sendSlackMessage (String message, MessageType msgType = INFO) {
    WiFiClientSecure *client = new WiFiClientSecure;
    HTTPClient https;
    
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
        // set https cert
        client -> setCACert(root_ca);
        client -> setInsecure();
      
        // Your Domain name with URL path or IP address with path
        https.begin(*client, serverName);
        
        // Specify content-type header
        https.addHeader("Content-Type", "application/json");
        
        // Data to send with HTTP POST
        String httpsRequestData = generateMessagePayload(message, msgType);
        // String httpsRequestData = "{\"channel\": \"#status-plant\", \"text\": \"hello\" }";
        
        // Send HTTP POST request
        int httpsResponseCode = https.POST(httpsRequestData);

        if (httpsResponseCode > 0) {
            String payload = https.getString();

            // file found at server
            if (httpsResponseCode == HTTP_CODE_OK || httpsResponseCode == HTTP_CODE_MOVED_PERMANENTLY) {
                Serial.println("Sent slack message: " + message);
            } else {
                Serial.println("Error: HTTP Response Code " + String(httpsResponseCode));
                Serial.println("Response: " + payload);
            }
        } else {
            Serial.printf("Error: %s\n", https.errorToString(httpsResponseCode).c_str());  
        }
          
        // Free resources
        https.end();
    } else {
        Serial.println("WiFi Disconnected");
    }

    // deallocate
    delete client;
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
    WiFi.begin(ssid, password);
    Serial.println("Connecting ...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
   
    // Random seed is a number used to initialize a pseudorandom number generator
    randomSeed(analogRead(33));

    sendSlackMessage(":white_check_mark: Plantbot booted up successfully", BOOT);
}

String generateMessagePayload (String message, MessageType msgType) {
    if (msgType == BOOT) {
        return generateChannelMessage(
            "'blocks': [{'type': 'header','text': {'type': 'plain_text','text': '" + message + "','emoji': true}}]" \ 
        );
    } else if (msgType == INFO) {
        return generateChannelMessage(
            "'text': '" + message + "'"
        );
    }
}

String generateChannelMessage (String jsonContent) {
    return (
        "{'channel': '#" + String(SLACK_INFO_CHANNEL_NAME) + "', " + jsonContent + "}"
    );
}

void loop () {
    unsigned long currentTime = millis();
    
    if (Serial2.available()) {
        String rxInput = Serial2.readString();
        Serial.println("Received data from Arduino: " + rxInput);
        sendSlackMessage(rxInput, INFO);
    }

    if ((currentTime - lastInfoTime) > timerAliveDelay) {
        sendSlackMessage(":wave: Still Alive ...", INFO);  
        lastInfoTime = currentTime;
    }
}
