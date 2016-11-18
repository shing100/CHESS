/*******************************
 * Safe program
 *
 * @author Team CHESS
 * @version v.1.1 Sep 28, 2016
 *
 * -------- MODE CODE --------
 * MODE_CONTROL : 아두이노 제어 명령
 * MODE_STATE : 아두이노 상태 조회 명령
 * MODE_CHECK : 센서 확인 명령
 *
 * -------- STATE CODE --------
 * JSON 명령 에러 : 10X
 * 아두이노 내부 에러 : 12X
 *
 * SUCCESS : 명령 실행 성공
 * WRONG_DEVICE_NAME_ERROR : 아두이노 이름 다름
 * WRONG_ACTION_ERROR : 정의되지 않은 action 코드 수신
 * INTERNAL_COMMAND_ERROR : 아두이노 명령 실행 오류
 ********************************/

#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <MemoryFree.h>

#define DEVICE_ID   5

#define SERVER_IP_ADDRESS "192.168.0.4"
#define HTTP_CONTENT_TYPE "Content-Type: application/x-www-form-urlencoded"
#define HTTP_CONTENT_LENGTH "Content-Length: "

#define MODE_CONTROL    1
#define MODE_STATE      2
#define MODE_CHECK      3

#define WRONG_DEVICE_NAME_ERROR 100
#define WRONG_ACTION_ERROR      101
#define INTERNAL_COMMAND_ERROR  120
#define SUCCESS 200

#define DETECT_SENSOR_PIN   A0

#define trigPin     2
#define echoPin     3
#define buzzerPin   4

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(9999);
EthernetClient client;

const String deviceName = "intrusionsensor";
boolean microwaveState = false; // 침입감지 동작 여부
boolean sensorState = false;
boolean detect = false; // 침입감지 여부

boolean syncState = false;     // ip 싱크 여부

void initDevice();      // 장비 초기화
void syncIpAddress(); // ip주소 동기화

long microsecondsToCentimeters(long microseconds);  // 초음파 센서 거리 측정값 반환
long getDistance(); // 초음파 센서 거리 측정
void detectIntrusion(); // 초음파 센서 동작

void ringBuzzer();  // 경보음 동작

void addDetectLog();    // 탐지 정보 DB서버에 등록

String readRequestToString(); // 클라이언트로부터 요청 수신하여 String 리턴
byte checkError(JsonObject& jsonObject); // 클라이언트에서 받은 메시지 검증
byte getAction(JsonObject& jsonObject);  // 클라이언트로 받은 명령값 리턴 (MODE_CONTROL, MODE_STATE, MODE_CHECK)

void remoteControlAction(JsonObject& jsonObject); // 클라이언트에서 온 명령 수행
byte doControl(JsonObject& jsonObject);  // Control 명령 수행
void convertDetectStatus();              // check 명령 수행
void sendErrorMessage(byte errCode);     // 클라이언트로 에러 메시지 전송
void sendControlResultMessage(JsonObject& jsonObject);  // 클라이언트로 control 명령 결과 전송
void sendStatusMessage();    // 클라이언트로 아두이노 상태 값 전송

void errPrintToSerial(byte errCode, String message); // 시리얼 모니터 에러 출력
String getIpAddress();    // Convert ipAddress to String
void printGetMessage(String rcvMessage); // 클라이언트 메시지 확인

void setup() {
    initDevice();
}

void loop() {
    detectIntrusion();

    client = server.available();

    if(client) {
        StaticJsonBuffer<200> jsonBuffer;
        String rcvMessage;
        byte errCode;

        rcvMessage = readRequestToString();

        printGetMessage(rcvMessage);

        JsonObject& jsonObject = jsonBuffer.parseObject(rcvMessage);
        errCode = checkError(jsonObject);

        if(errCode != SUCCESS) {
            sendErrorMessage(errCode);
        } else {
            switch (getAction(jsonObject)) {
                case MODE_CONTROL:
                    remoteControlAction(jsonObject);
                    break;
                case MODE_CHECK:
                    convertDetectStatus();
                    break;
                case MODE_STATE :
                    sendStatusMessage();
                    break;
            }

            Serial.print("freeMemory() = ");
            Serial.println(freeMemory());
        }

        client.stop();
    }
}

/**
 * 아두이노 실행 시 Serial, Ethernet, pinMode 정보 초기화
 *
 * method name : initDevice
 * @param void
 * @return void
 */
void initDevice() {
    Serial.begin(9600);
    Ethernet.begin(mac);
    server.begin();
    client.setTimeout(2000);
    syncIpAddress();
    
    Serial.print("Server Start : ");
    Serial.println(Ethernet.localIP());

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(buzzerPin, OUTPUT);

    digitalWrite(buzzerPin, LOW);
}

/**
 * 아두이노가 할당 받은 iP주소를 메인 서버 DB와 동기화
 *
 * method name : syncIpAddress
 * @param void
 * @return void
 */
void syncIpAddress() {
    String data = "id=";
    data += DEVICE_ID;
    data += "&ipAddress=";
    data += getIpAddress();

    if(client.connect(SERVER_IP_ADDRESS, 8080)) {
        client.println("POST /device/sync HTTP/1.1");
        client.print("Host: ");
        client.println(SERVER_IP_ADDRESS);
        client.println(HTTP_CONTENT_TYPE);
        client.print(HTTP_CONTENT_LENGTH);
        client.println(data.length());
        client.println();
        client.println(data);
        client.println();

        client.stop();

        syncState = true;
    }
}

/**
 * 초음파 센서를 통해 읽어들인 microseconds를 거리 값으로 변환
 *
 * method name : microsecondsToCentimeters
 * @param long microseconds
 * @return long 변환된 cm값 반환
 */
long microsecondsToCentimeters(long microseconds) {
    return microseconds / 29 / 2;
}

/**
 * 초음파 센서 거리 측정
 *
 * method name : getDistance
 * @param void
 * @return long 측정된 값 반환 cm
 */
long getDistance() {
    long duration;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);

    return microsecondsToCentimeters(duration);
}

/**
 * 초음파 센서를 작동하여 침입 감지
 *
 * method name : detectIntrusion
 * @param void
 * @return void
 */
void detectIntrusion() {
    if(analogRead(DETECT_SENSOR_PIN) <= 1000) {
        sensorState = false;
        return;
    } else {
        if(microwaveState) {
            if(getDistance() < 15) {
                Serial.println("Detect!!");
                detect = true;
                addDetectLog();
            }
        } else {
            detect = false;
        }

        sensorState = true;
    }

    if(detect)
        ringBuzzer();
}

/**
 * 침입 감지시 부저 동작
 *
 * method name : ringBuzzer
 * @param void
 * @return void
 */
void ringBuzzer() {
    int pitchLow = 200;
    int pitchHigh = 1000;
    int pitchStep = 10;
    int currentPitch = 0;
    int delayTime = 10;

    for(;currentPitch<=pitchHigh; currentPitch+=pitchStep) {
        tone(buzzerPin, currentPitch, 10);
        delay(delayTime);
    }
    for(;currentPitch>=pitchLow; currentPitch-=pitchStep) {
        tone(buzzerPin, currentPitch, 10);
        delay(delayTime);
    }
}

/**
 * 탐지 정보 DB 서버에 저장
 *
 * method name : addDetectLog
 * @param void
 * @return void
 */
void addDetectLog() {
    if(client.connect(SERVER_IP_ADDRESS, 8080)) {
        client.println("PUT /log/detect/" + deviceName +" HTTP/1.1");
        client.print("Host: ");
        client.println(SERVER_IP_ADDRESS);
        client.println(HTTP_CONTENT_TYPE);
        client.print(HTTP_CONTENT_LENGTH);
        client.println(0);
        client.println();
        client.println("");
        client.println();

        client.stop();
    }
}

/**
 * 클라이언트로부터 요청을 받은 값 String으로 반환
 *
 * method name : readRequestToString
 * @param void
 * @return String 클라이언트 요청 String 리턴
*/
String readRequestToString() {
    String message;

    if(client) {
        while(client.available()) {
            char c = client.read();
            message += c;

            if(c == '\0')
                break;
        }
    }
    return message;
}

/**
 * 클라이언트에서 받은 메시지 에러 체크
 *
 * method name : checkError
 * @param JsonObject& jsonObject
 * @return byte
 *         오류 발생 X -> 0리턴
 *         오류 발생 -> 오류코드 리턴
 */
byte checkError(JsonObject& jsonObject) {
    String dName = jsonObject["deviceName"];

    if(!dName.equals(deviceName))
        return WRONG_DEVICE_NAME_ERROR;
    else if(getAction(jsonObject) == WRONG_ACTION_ERROR)
        return WRONG_ACTION_ERROR;
    else
        return SUCCESS;
}

/**
 * 클라이언트에서 받은 명령 분석하여 값 리턴
 *
 * method name : getAction
 * Action : control -> MODE_CONTROL
 *          state -> MODE_STATE
 *          check -> MOD_CHECK
 * @param JsonObject& jsonObject
 * @return byte
 */
byte getAction(JsonObject& jsonObject) {
    String action = jsonObject["action"];

    if(action.equals("control"))
        return MODE_CONTROL;
    else if(action.equals("state"))
        return MODE_STATE;
    else if(action.equals("check"))
        return MODE_CHECK;
    else
        return WRONG_ACTION_ERROR;
}

/**
 * 클라이언트에서 받은 control 수행 후 결과 값 클라이언트로 전송
 *
 * method name : remoteControlAction
 * @param JsonObejct& jsonObject
 * @return void
 */
void remoteControlAction(JsonObject& jsonObject) {
    byte result;

    result = doControl(jsonObject);

    if(result == SUCCESS)
        sendControlResultMessage(jsonObject);
}

/**
 * Control 명령 수행
 *
 * method name : doControl
 * @param JsonObject& jsonObject
 * @return byte 명령 수행 결과
 */
byte doControl(JsonObject& jsonObject) {
    boolean command = jsonObject["message"]["command"];

    microwaveState = command;

    if(!microwaveState)
        detect = false;

    return SUCCESS;
}

/**
 * check 명령 수행 (detect 상태 false로 변환)
 *
 * method name : convertDetectStatus
 * @param void
 * @return void
 *
 */
void convertDetectStatus() {
    StaticJsonBuffer<50> jsonBuffer;
    JsonObject& message = jsonBuffer.createObject();

    detect = false;

    message["status"] = "success";

    message.prettyPrintTo(client);
}

/**
 * 클라이언트로 에러 메시지 전송
 *
 * method name : sendErrorMessage
 * @param byte errCode
 * @return void
 */
void sendErrorMessage(byte errCode) {
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& error = jsonBuffer.createObject();
    String message;

    switch (errCode) {
        case WRONG_DEVICE_NAME_ERROR:
            message = "wrong device name";
            break;
        case WRONG_ACTION_ERROR:
            message = "wrong action";
            break;
        case INTERNAL_COMMAND_ERROR:
            message = "internal command error";
            break;
    }

    error["status"] = "error";

    JsonObject& errMessage = error.createNestedObject("errors");

    errMessage["code"] = errCode;
    errMessage["message"] = message;

    errPrintToSerial(errCode, message);

    error.prettyPrintTo(client);
}

/**
 * control action 결과 값 클라이언트로 전송
 *
 * method name : sendControlResultMessage
 * @param JsonObject& jsonObject
 */
void sendControlResultMessage(JsonObject& jsonObject) {
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& result = jsonBuffer.createObject();

    result["status"] = "success";

    JsonObject& data = result.createNestedObject("data");
    data["deviceName"] = deviceName;
    data["ipAddress"] = getIpAddress();
    data["command"] = jsonObject["message"]["command"];
    data["state"] = microwaveState;

    result.prettyPrintTo(client);
}

/**
 * 클라이언트로부터 state 명령을 받으면 현재 아두이노의 상태 값 전송
 *
 * method name : sendStateResultMessage
 * @param void
 * @return void
 */
void sendStatusMessage() {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& state = jsonBuffer.createObject();

    state["status"] = "success";

    JsonObject& data = state.createNestedObject("data");
    data["deviceName"] = deviceName;
    data["ipAddress"] = getIpAddress();
    data["power"] = sensorState;
    data["state"] = microwaveState;

    if(microwaveState)
        data["detect"] = detect;

    state.prettyPrintTo(client);
}

/**
 * ip주소를 String 타입으로 변환
 *
 * method name : getIpAddress
 * @param void
 * @return String
 */
String getIpAddress() {
    String ipAddress = "";

    for(byte i=0; i<4; i++)
        ipAddress += (i != 3) ? String(Ethernet.localIP()[i]) + "." : String(Ethernet.localIP()[i]);

    return ipAddress;
}

/**
 * 아두이노 에러 메시지 시리얼 출력
 *
 * method name : errPrintToSerial
 * @param errCode
 * @param message
 */
void errPrintToSerial(byte errCode, String message) {
    Serial.print("Error Code : ");
    Serial.println(errCode);
    Serial.print("Message : ");
    Serial.println(message);
}

/**
 * 디버그용 함수 (클라이언트 메시지 확인)
 *
 * method name : printGetMessage
 * @param rcvMessage
 * @return void
 */
void printGetMessage(String rcvMessage) {
    Serial.println(rcvMessage);
}
