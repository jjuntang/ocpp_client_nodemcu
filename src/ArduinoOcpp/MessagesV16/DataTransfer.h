// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef DATATRANSFER_H
#define DATATRANSFER_H

#include <ArduinoOcpp/Core/OcppMessage.h>

namespace ArduinoOcpp {
namespace Ocpp16 {

class DataTransfer : public OcppMessage {
private:
  String msg;
  String messageId;
public:
  // DataTransfer();

  DataTransfer(String &msg, String &messageId);

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);

  void processReq(JsonObject payload);

  DynamicJsonDocument* createConf();
};

} //end namespace Ocpp16
} //end namespace ArduinoOcpp
#endif
