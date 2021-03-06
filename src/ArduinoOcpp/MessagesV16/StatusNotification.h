// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef STATUSNOTIFICATION_H
#define STATUSNOTIFICATION_H

#include <ArduinoOcpp/Core/OcppMessage.h>
#include <ArduinoOcpp/Tasks/ChargePointStatus/OcppEvseState.h>
#include <ArduinoOcpp/Core/OcppTime.h>

namespace ArduinoOcpp {
namespace Ocpp16 {

class StatusNotification : public OcppMessage {
private:
    int connectorId = 1;
    OcppEvseState currentStatus = OcppEvseState::NOT_SET;
    OcppTimestamp otimestamp;
    const char *errorCode = NULL; //NULL is equivalent to "NoError"
public:
    StatusNotification(int connectorId, OcppEvseState currentStatus, const OcppTimestamp &otimestamp, const char *errorCode = NULL);

    StatusNotification();

    const char* getOcppOperationType();

    DynamicJsonDocument* createReq();

    void processConf(JsonObject payload);

    void processReq(JsonObject payload);

    DynamicJsonDocument* createConf();
};

} //end namespace Ocpp16
} //end namespace ArduinoOcpp
#endif
