// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#include <ArduinoOcpp/MessagesV16/DataTransfer.h>
#include <ArduinoOcpp/Core/OcppEngine.h>
#include <Variants.h>
#include "test.h"

using ArduinoOcpp::Ocpp16::DataTransfer;

// DataTransfer::DataTransfer() {
//     Serial.print(F("[DataTransfer]\n"));
// }
String gReservationId = String('\0');

DataTransfer::DataTransfer(String &msg, String &messageId) {
    Serial.print(F("[DataTransfer]msg : "));
    Serial.print(msg);
    Serial.print(F("\n"));    
    this->msg = msg;
    this->messageId = messageId;
}

const char* DataTransfer::getOcppOperationType(){
    return "DataTransfer";
}

static char temp[128];        
DynamicJsonDocument* DataTransfer::createReq() {
  Serial.print(F("[DataTransfer]createReq!\n"));
  DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(2) + (msg.length() + 1));
  JsonObject payload = doc->to<JsonObject>();

    payload["vendorId"] = "CustomVendor";  
//   payload["messageId"] = messageId;
//   payload["data"] = msg;

    // payload["messageId"] = "requestUnitPrice";
    // payload["data"] = "{\"uid\":\" \"}";

    // payload["messageId"] = "requestAuthUnitPrice";
    // payload["data"] = "{\"uid\":\"1010010174790089\"}";

    // payload["messageId"] = "unTransmittedData";
    // payload["data"] = "[2, \"20220610123456\", \"StartTransaction\", {\"connectorId\":1,\"idTag\":\"1010010174790089\",\"meterStart\":0,\"reservationId\":0,\"timestamp\":\"2022-06-10T12:34:56Z\"}]";

// #if (TEST_MODE == TEST_NON_MEMBER)
//     payload["messageId"] = "prePayment";
//     payload["data"] = "{\"connectorId\":2,\"paymentType\":\"Approval\",\"result\":\"Success\",\"paymentPrice\":\"1000\",\"tid\":\"\",\"approvalNo\":\"08740610\",\"vanInfo\":\"SmartRo\",\"pgNo\":\"ktevc0002m01032206021919180866\",\"timestamp\":\"2022-06-02T19:19:18Z\"}";
// #endif 

#if (TEST_DATA_TRANSFER == TEST_UNPLUGGED)
    payload["messageId"] = "unplugged";    

    ConnectorStatus *connector = getConnectorStatus(1);    
    if (connector) {                
        memset(temp, 0, 128);
        sprintf(temp, "{\"transactionId\":%d,\"connectorId\":1,\"meterStop\":10}", connector->getTransactionIdSync());
        // String temp1("{\"transactionId\":" + connector->getTransactionIdSync() + "\"connectorId\":1,\"meterStop\":10}");
        // Serial.print(temp1);
        payload["data"] = (const char*)temp;
    }
    else {
        payload["data"] = "{\"transactionId\":12345678,\"connectorId\":1,\"meterStop\":10}";
    }

#endif 

#if (TEST_DATA_TRANSFER == TEST_INIT_CP_INFO)
    payload["messageId"] = "initCpInfo";
    payload["data"] = "{\"manufacturerCode\":\"43\",\"modemManufacturerCode\":\"43\",\"modemTelNumber\":\"01012345678\",\"paymentModuleIp\":\"123.456.789.001\",\"paymentModulePort\":1234,\"server\":\"ocpp.chargev.co.kr\",\"serverPort\":9010,\"savedLocalMemberCount\":5}";
#endif 

    // payload["messageId"] = "remoteStopTransaction";
    // payload["data"] = "{\"transactionId\":12345678,\"connectorId\":1}";

    Serial.print(F("[DataTransfer createReq]msg1 : "));
    Serial.print(messageId);
    Serial.print(F("\n"));    

    Serial.print(F("[DataTransfer createReq]msg2 : "));
    Serial.print(msg);
    Serial.print(F("\n"));    

  return doc;
}

void DataTransfer::processConf(JsonObject payload){
    String status = payload["status"] | "Invalid";

    if (status.equals("Accepted")) {
        if (DEBUG_OUT) Serial.print(F("[DataTransfer] Request has been accepted!\n"));
        String data = payload["data"];
        Serial.printf("%s\r\n", data.c_str());
        char reservationId[16] = {0, };
        memcpy(reservationId, data.c_str(), 22);
        Serial.printf("reservationId : %s\r\n", &reservationId[18]);
        String temp(&reservationId[18]);
        gReservationId = temp;
        Serial.printf("reservationId String : %s\r\n", gReservationId.c_str());
    } else {
        Serial.print(F("[DataTransfer] Request has been denied!"));
    }
}


void DataTransfer::processReq(JsonObject payload) {
    Serial.print(F("[DataTransfer]processReq!\n"));
}

DynamicJsonDocument* DataTransfer::createConf(){
    DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
    JsonObject payload = doc->to<JsonObject>();
    payload["status"] = "NotSupported";
    return doc;
}
