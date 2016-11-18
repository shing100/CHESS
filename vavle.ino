/*******************************
 * Valve control program
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
 * 장비 명령 에러 : 11X
 * 아두이노 내부 에러 : 12X
 *
 * SUCCESS : 명령 실행 성공
 * WRONG_DEVICE_NAME_ERROR : 아두이노 이름 다름
 * WRONG_ACTION_ERROR : 정의되지 않은 action 코드 수신
 * INTERNAL_COMMAND_ERROR : 아두이노 명령 실행 오류
 ********************************/

#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson/ArduinoJson.h>
#include <MemoryFree-master/MemoryFree.h>

#define DEVICE_ID    2
#define DEVICE_ID2   3

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

#define VALVE_ON  true
#define VALVE_OFF false

#define DETECT_SENSOR_PIN   A0

#define valveSW 2
#define valve   3
#define sensor  7

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEB};
EthernetServer server(9999);
EthernetClient client;

boolean swState = false;        // 현재 스위치 상태 (On, Off)
boolean valveState = false;     // 현재 밸브 상태 (On Off)
boolean powerState = false;    // 화재감지 센서 동작 상태
boolean detect = false;         // 화재감지 여부
const String _deviceName[] = {"valve", "firesensor"};
String deviceName;

boolean syncState = false;     // ip 싱크 여부

void initDevice();      // 장비 초기화
void syncIpAddress(); // ip주소 동기화
boolean requestSync(byte deviceId);

void manualControl();   // 장비 수동 제어
void valveOnOff();      // 스위치 상태값에 따라 벨브 On Off
void detectFire();      // 화재 감지

void addDetectLog();    // 탐지 정보 DB서버에 등록

String readRequestToString(); // 클라이언트로부터 요청 수신하여 String 리턴
byte checkError(JsonObject& jsonObject); // 클라이언트에서 받은 메시지 검증
byte getAction(JsonObject& jsonObject);  // 클라이언트로 받은 명령값 리턴 (MODE_CONTROL, MODE_STATE, MODE_CHECK)

void remoteControlAction(JsonObject& jsonObject); // 클라이언트에서 온 명령 받아 doCotrol 호출 후 결과 값 클라이언트에 전송
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
    if(!syncState)
        syncIpAddress();

    client = server.available();
    detectFire();

    if(client) {
        StaticJsonBuffer<200> jsonBuffer;
        String rcvMessage;
        byte errCode;

        rcvMessage = readRequestToString();

        printGetMessage(rcvMessage);

        JsonObject& jsonObject = jsonBuffer.parseObject(rcvMessage);
        errCode = checkError(jsonObject);

        if(errCode != SUCCESS) {            // 에러 존재시 에러 코드 전송
            sendErrorMessage(errCode);
        } else {                            // 정상 실행
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
    } else {
        manualControl();
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


    pinMode(valveSW, INPUT);
    pinMode(valve, OUTPUT);
    pinMode(sensor, INPUT);

    digitalWrite(valve, LOW);
}

/**
 * 아두이노가 할당 받은 iP주소를 메인 서버 DB와 동기화
 *
 * method name : syncIpAddress
 * @param void
 * @return void
 */
void syncIpAddress() {
    if(requestSync(DEVICE_ID) && requestSync(DEVICE_ID2))
        syncState = true;
}

boolean requestSync(byte deviceId) {
    String data = "id=";
    data += deviceId;
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

        return true;
    }

    return false;
}

/**
 * 스위치 값을 읽어 스위치 상태 값 변경
 *
 * method name : manualControl
 * @param void
 * @return void
 */
void manualControl() {
    if(digitalRead(valveSW) == HIGH) {
        while(digitalRead(valveSW) == HIGH);

        swState = !swState;
    }

    valveOnOff();
}

/**
 * swState 값에 따라 밸브 On Off
 *
 * method name : valveOnOff
 * @param void
 * @return void
 */
void valveOnOff() {
    if(swState) {
        digitalWrite(valve, HIGH);
        valveState = VALVE_ON;
    } else {
        digitalWrite(valve, LOW);
        valveState = VALVE_OFF;
    }
}

/**
 * 화재감지 함수
 *
 * method name : detectFire
 * @param void
 * @return void
 */
void detectFire() {
    if(analogRead(DETECT_SENSOR_PIN) <= 1000) {
        powerState = false;
        return;
    } else {
        powerState = true;
        delay(5);   // 화재센서 로드 딜레이

        if(digitalRead(sensor) == LOW && analogRead(DETECT_SENSOR_PIN) >= 1000) {
            if(!detect)
                addDetectLog();

            detect = true;
            swState = VALVE_OFF;
        }
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
        client.println("PUT /log/detect/" + _deviceName[1] +" HTTP/1.1");
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
    String ipAddress = jsonObject["ipAddress"];
    boolean deviceNameCheck = false;

    for(byte i=0; i<2; i++) {
        if(dName.equals(_deviceName[i])) {
            deviceNameCheck = true;
            deviceName = _deviceName[i];
            break;
        }
    }

    if(!deviceNameCheck)
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
    else
        sendErrorMessage(result);
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
    boolean commandResult;

    swState = command;
    valveOnOff();

    commandResult = bitRead(PORTD, valve) == swState;

    if(commandResult)
        return SUCCESS;
    else
        return INTERNAL_COMMAND_ERROR;
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
    data["state"] = valveState;

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
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& state = jsonBuffer.createObject();

    state["status"] = "success";

    JsonObject& data = state.createNestedObject("data");
    data["deviceName"] = deviceName;
    data["ipAddress"] = getIpAddress();

    if(deviceName.equals("valve")) {
        valveState = bitRead(PORTD, valve) == HIGH ? true : false;
        data["state"] = valveState;
    } else if(deviceName.equals("firesensor")) {
        data["state"] = powerState;
        data["detect"] = detect;
    }

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
//    Serial.print("Error Code : ");
//    Serial.println(errCode);
//    Serial.print("Message : ");
//    Serial.println(message);
}

/**
 * 디버그용 함수 (클라이언트 메시지 확인)
 *
 * method name : printGetMessage
 * @param rcvMessage
 * @return void
 */
void printGetMessage(String rcvMessage) {
//    Serial.println(rcvMessage);
}