// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef STOPTRANSACTION_H
#define STOPTRANSACTION_H

#include <ArduinoOcpp/Core/OcppMessage.h>
#include <ArduinoOcpp/Core/OcppTime.h>

namespace ArduinoOcpp {
namespace Ocpp16 {

class StopTransaction : public OcppMessage {
private:
    int connectorId = 1;
    float meterStop = 10.0f;
    OcppTimestamp otimestamp;
    String idTag;
public:
    StopTransaction();

    StopTransaction(int connectorId);

    StopTransaction(int connectorId, String &idTag);

    const char* getOcppOperationType();

    void initiate();

    DynamicJsonDocument* createReq();

    void processConf(JsonObject payload);

    void processReq(JsonObject payload);

    DynamicJsonDocument* createConf();
};

} //end namespace Ocpp16
} //end namespace ArduinoOcpp
#endif
