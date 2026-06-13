#pragma once

#include <Arduino.h>

#include "message/messageManager.h"
#include "message/messageService.h"

#include "runtimeConfigServiceMessage.h"
#include "runtimeConfigCommandService.h"

#include "config.h"

#include "LoraMesher.h"

class RuntimeConfigService : public MessageService {
public:
        /**
     * @brief Construct a new runtimeConfigService object
     *
     */    
    static RuntimeConfigService& getInstance() {
        static RuntimeConfigService instance;
        return instance;
    }

    ~RuntimeConfigService() {
        if (runtimeConfigCommandService != nullptr) {
            delete runtimeConfigCommandService;
        }
    }

    void init();

    String getJSON(DataMessage* message);

    DataMessage* getDataMessage(JsonObject data);

    void processReceivedMessage(messagePort port, DataMessage* message);

    RuntimeConfigCommandService* runtimeConfigCommandService = nullptr;

private:
    RuntimeConfigService() : MessageService(RuntimeConfig, "Runtime Config") {
        runtimeConfigCommandService = new RuntimeConfigCommandService();
        commandService = runtimeConfigCommandService;
    };

    size_t runtimeConfigMessageId = 0;

    LoraMesher::LoraMesherConfig backupConfig;

    const static int BW_ARRAY_SIZE = 9;
    const float allowedBwValues[BW_ARRAY_SIZE] = {10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250, 500};

    DataMessage* getModifyConfigMessage(JsonObject data);
    DataMessage* getBackupCurrentConfigMessage(JsonObject data);
    DataMessage* getRestoreFromBackupMessage(JsonObject data);

};