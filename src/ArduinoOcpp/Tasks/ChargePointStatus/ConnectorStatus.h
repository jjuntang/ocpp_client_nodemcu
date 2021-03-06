// matth-x/ArduinoOcpp
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef CONNECTOR_STATUS
#define CONNECTOR_STATUS

#include <ArduinoOcpp/MessagesV16/StatusNotification.h>
#include <ArduinoOcpp/Tasks/ChargePointStatus/OcppEvseState.h>
#include <ArduinoOcpp/Core/Configuration.h>

#define AVAILABILITY_OPERATIVE 2
#define AVAILABILITY_INOPERATIVE_SCHEDULED 1
#define AVAILABILITY_INOPERATIVE 0

namespace ArduinoOcpp {

class ConnectorStatus {
private:
    const int connectorId;
    OcppTime *ocppTime;

    std::shared_ptr<Configuration<int>> availability = NULL;

    bool authorized = false;
    String idTag = String('\0');
    bool transactionRunning = false;
    //int transactionId = -1;
    std::shared_ptr<Configuration<int>> transactionId = NULL;
    int transactionIdSync = -1;
    bool evDrawsEnergy = false;
    bool evseOffersEnergy = false;
    std::function<bool()> connectorPluggedSampler = NULL;
    //std::function<bool()> connectorFaultedSampler = NULL;
    std::vector<std::function<const char *()>> connectorErrorCodeSamplers;
    const char *getErrorCode();
    OcppEvseState currentStatus = OcppEvseState::NOT_SET;
    std::function<bool()> onUnlockConnector = NULL;
public:
    ConnectorStatus(int connectorId, OcppTime *ocppTime);

    //boolean requestAuthorization();
    void authorize();
    void authorize(String &idTag);
    void unauthorize();
    String &getIdTag();
    int getTransactionId();
    int getTransactionIdSync();
    uint16_t getTransactionWriteCount();
    void setTransactionId(int id);
    void setTransactionIdSync(int id);
    int getAvailability();
    void setAvailability(bool available);
    void boot();
    void startEvDrawsEnergy();
    void stopEvDrawsEnergy();
    void startEnergyOffer();
    void stopEnergyOffer();
    void setConnectorPluggedSampler(std::function<bool()> connectorPlugged);
    //void setConnectorFaultedSampler(std::function<bool()> connectorFaulted);
    void addConnectorErrorCodeSampler(std::function<const char*()> connectorErrorCode);

    void saveState();
    //void recoverState();

    Ocpp16::StatusNotification *loop();

    OcppEvseState inferenceStatus();

    void setOnUnlockConnector(std::function<bool()> unlockConnector);
    std::function<bool()> getOnUnlockConnector();
};

} //end namespace ArduinoOcpp
#endif
