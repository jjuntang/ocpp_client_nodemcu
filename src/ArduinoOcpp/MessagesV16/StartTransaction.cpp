// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#include <ArduinoOcpp/MessagesV16/StartTransaction.h>
#include <ArduinoOcpp/Core/OcppEngine.h>
#include <ArduinoOcpp/Tasks/Metering/MeteringService.h>

#include <Variants.h>
#include "test.h"

using ArduinoOcpp::Ocpp16::StartTransaction;
extern String gReservationId;
extern int gTransactionId;

StartTransaction::StartTransaction(int connectorId) : connectorId(connectorId) {
    this->idTag = String('\0');
}

StartTransaction::StartTransaction(int connectorId, String &idTag) : connectorId(connectorId) {
    this->idTag = String(idTag);
}

const char* StartTransaction::getOcppOperationType(){
    return "StartTransaction";
}

void StartTransaction::initiate() {
    MeteringService* meteringService = getMeteringService();
    if (meteringService != NULL) {
        meterStart = meteringService->readEnergyActiveImportRegister(connectorId);
    }

    OcppTime *ocppTime = getOcppTime();
    if (ocppTime) {
        otimestamp = ocppTime->getOcppTimestampNow();
    } else {
        otimestamp = MIN_TIME;
    }

    if (idTag.isEmpty()) {
        ChargePointStatusService *cpss = getChargePointStatusService();
        if (cpss != NULL && cpss->existsUnboundAuthorization()) {
            this->idTag = String(cpss->getUnboundIdTag()); 
        } else {
            //The CP is not authorized. Try anyway, let the CS decide what to do ...
            this->idTag = String("A0-00-00-00"); //Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
        }
    }

    ChargePointStatusService *cpStatusService = getChargePointStatusService();
    if (cpStatusService) {
        cpStatusService->bindAuthorization(connectorId);
    }

    ConnectorStatus *connector = getConnectorStatus(connectorId);
    if (connector != NULL){
        if (connector->getTransactionId() >= 0) {
            Serial.print(F("[StartTransaction] Warning: started transaction while OCPP already presumes a running transaction\n"));
        }
        connector->setTransactionId(0); //pending
        transactionRev = connector->getTransactionWriteCount();
    }

    if (DEBUG_OUT) Serial.println(F("[StartTransaction] StartTransaction initiated!"));
}

DynamicJsonDocument* StartTransaction::createReq() {
    DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (JSONDATE_LENGTH + 1) + (idTag.length() + 1));
    JsonObject payload = doc->to<JsonObject>();

    payload["connectorId"] = connectorId;
    if (meterStart >= 0.f) {
        payload["meterStart"] = meterStart;
    }

    if (otimestamp > MIN_TIME) {
        char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
        otimestamp.toJsonString(timestamp, JSONDATE_LENGTH + 1);
        payload["timestamp"] = timestamp;
    }

    // payload["idTag"] = idTag;
#if (TEST_MODE == TEST_NON_MEMBER)    
    payload["idTag"] = "5959595959591004";
#endif 

#if (TEST_MODE == TEST_MEMBER)
    payload["idTag"] = "1010010174790089";
#endif 

    Serial.printf("reservationId : %s\r\n", gReservationId.c_str());
    payload["reservationId"] = gReservationId.c_str();    
    return doc;
}

void StartTransaction::processConf(JsonObject payload) {

    const char* idTagInfoStatus = payload["idTagInfo"]["status"] | "Invalid";
    int transactionId = payload["transactionId"] | -1;

    if (!strcmp(idTagInfoStatus, "Accepted")) {
        if (DEBUG_OUT) Serial.print(F("[StartTransaction] Request has been accepted!\n"));

        ConnectorStatus *connector = getConnectorStatus(connectorId);
        if (connector){
            if (transactionRev == connector->getTransactionWriteCount()) {
                
                Serial.printf("transaction Id : %d\r\n", transactionId);
                gTransactionId = transactionId;

                connector->setTransactionId(transactionId);
            }
            connector->setTransactionIdSync(transactionId);
        }
    } else {
        Serial.print(F("[StartTransaction] Request has been denied!\n"));
        ConnectorStatus *connector = getConnectorStatus(connectorId);
        if (connector){
            if (transactionRev == connector->getTransactionWriteCount()) {
                connector->setTransactionId(-1);
                connector->unauthorize();
            }
            connector->setTransactionIdSync(-1);
        }
    }
}


void StartTransaction::processReq(JsonObject payload) {

  /**
   * Ignore Contents of this Req-message, because this is for debug purposes only
   */

}

DynamicJsonDocument* StartTransaction::createConf(){
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2));
  JsonObject payload = doc->to<JsonObject>();

  JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
  idTagInfo["status"] = "Accepted";
  payload["transactionId"] = 123456; //sample data for debug purpose

  return doc;
}
