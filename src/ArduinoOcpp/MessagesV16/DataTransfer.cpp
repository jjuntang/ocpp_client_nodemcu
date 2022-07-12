// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#include <ArduinoOcpp/MessagesV16/DataTransfer.h>
#include <ArduinoOcpp/Core/OcppEngine.h>
#include <Variants.h>

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

    payload["messageId"] = "prePayment";
    payload["data"] = "{\"connectorId\":2,\"paymentType\":\"Approval\",\"result\":\"Success\",\"paymentPrice\":\"1000\",\"tid\":\"\",\"approvalNo\":\"08740590\",\"vanInfo\":\"SmartRo\",\"pgNo\":\"ktevc0002m01032206021919180866\",\"timestamp\":\"2022-06-02T19:19:18Z\"}";

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
