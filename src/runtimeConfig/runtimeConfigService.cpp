#include "runtimeConfigService.h"

static const char* RUNTIME_CONFIG_TAG = "RuntimeConfigService";

void RuntimeConfigService::init() {
    ESP_LOGV(RUNTIME_CONFIG_TAG, "Entered RuntimeConfig init");
    backupConfig = LoraMesher::getInstance().getConfig();
}

String RuntimeConfigService::getJSON(DataMessage* message) {
    RuntimeConfigCommandMessage* runtimeConfigCommandMessage = (RuntimeConfigCommandMessage*)message;

    StaticJsonDocument<1000> doc;

    JsonObject root = doc.to<JsonObject>();

    runtimeConfigCommandMessage->serialize(root);

    String json;
    serializeJson(doc, json);

    return json;
}

DataMessage* RuntimeConfigService::getDataMessage(JsonObject data) {
    switch ((RuntimeConfigCommand)data["runtimeConfigCommand"]) {
        case RuntimeConfigCommand::ModifyConfig:
            ESP_LOGI(RUNTIME_CONFIG_TAG, "Entered ModifyConfig");
            return getModifyConfigMessage(data);
            break;
        case RuntimeConfigCommand::BackupCurrentConfig:
            ESP_LOGI(RUNTIME_CONFIG_TAG, "Entered BackupCurrentConfig");
            return getBackupCurrentConfigMessage(data);
            break;
        case RuntimeConfigCommand::RestoreFromBackup:
            ESP_LOGI(RUNTIME_CONFIG_TAG, "Entered RestoreFromBackup");
            return getRestoreFromBackupMessage(data);
            break;
    }

    ESP_LOGI(RUNTIME_CONFIG_TAG, "Unknown Runtime Config command: %d", data["runtimeConfigCommand"].as<uint8_t>());

    return nullptr;
}

DataMessage* RuntimeConfigService::getModifyConfigMessage(JsonObject data) {
    ModifyConfigMessage* modification = new ModifyConfigMessage();
    modification->deserialize(data);
    modification->messageSize = sizeof(ModifyConfigMessage) - sizeof(DataMessageGeneric);
    return ((DataMessage*)modification);
}

DataMessage* RuntimeConfigService::getBackupCurrentConfigMessage(JsonObject data) {
    BackupCurrentConfigMessage* make_backup = new BackupCurrentConfigMessage();
    make_backup->deserialize(data);
    make_backup->messageSize = sizeof(BackupCurrentConfigMessage) - sizeof(DataMessageGeneric);
    return ((DataMessage*)make_backup);
}

DataMessage* RuntimeConfigService::getRestoreFromBackupMessage(JsonObject data) {
    RestoreFromBackupMessage* restore_backup = new RestoreFromBackupMessage();
    restore_backup->deserialize(data);
    restore_backup->messageSize = sizeof(RestoreFromBackupMessage) - sizeof(DataMessageGeneric);
    return ((DataMessage*)restore_backup);
}

void RuntimeConfigService::processReceivedMessage(messagePort port, DataMessage* message) {
    RuntimeConfigCommandMessage* runtimeConfigCommandMessage = (RuntimeConfigCommandMessage*)message;
    ModifyConfigMessage* modifyConfigMessage;
    LoraMesher::LoraMesherConfig config;

    boolean allowed;
    int i;

    switch(runtimeConfigCommandMessage->runtimeConfigCommand) {
        case RuntimeConfigCommand::ModifyConfig:
            modifyConfigMessage = (ModifyConfigMessage*)runtimeConfigCommandMessage;
            ESP_LOGI(RUNTIME_CONFIG_TAG, "Received ModifyConfig message");

            config = LoraMesher::getInstance().getConfig();
            
            // Modify LoRa pins
            if (modifyConfigMessage->loraCs != 0) config.loraCs = modifyConfigMessage->loraCs;
            if (modifyConfigMessage->loraRst != 0) config.loraRst = modifyConfigMessage->loraRst;
            if (modifyConfigMessage->loraIrq != 0) config.loraIrq = modifyConfigMessage->loraIrq;
            if (modifyConfigMessage->loraIo1 != 0) config.loraIo1 = modifyConfigMessage->loraIo1;

            if (modifyConfigMessage->freq >= 137.0 && modifyConfigMessage->freq <= 1020.0) config.freq = modifyConfigMessage->freq;

            allowed = false;
            i = 0;
            while (!allowed && i < BW_ARRAY_SIZE) {
                if (modifyConfigMessage->bw == allowedBwValues[i]) allowed = true;
                i++;
            }
            if (allowed) config.bw = modifyConfigMessage->bw;

            if (modifyConfigMessage->sf >= 6 && modifyConfigMessage->sf <= 12) config.sf = modifyConfigMessage->sf;

            if (modifyConfigMessage->cr >= 5 && modifyConfigMessage->sf <= 8) config.cr = modifyConfigMessage->cr;

            if (modifyConfigMessage->syncWord != 0 && modifyConfigMessage->syncWord != (uint8_t)0x34) config.syncWord = modifyConfigMessage->syncWord;

            if (modifyConfigMessage->power >= 2 && modifyConfigMessage->sf <= 17) config.power = modifyConfigMessage->power;

            if (modifyConfigMessage->preambleLength != 0) config.preambleLength =  modifyConfigMessage->preambleLength;
            
            if (modifyConfigMessage->max_packet_size >= 13 && modifyConfigMessage->max_packet_size <= 255) config.max_packet_size = modifyConfigMessage->max_packet_size;

            ESP_LOGI(RUNTIME_CONFIG_TAG, 
             "New LoraMesher config: CS: %d, RST: %d, IRQ: %d, IO1: %d, Freq: %f, Bandwidth: %f, Spreading Factor: %d, Coding Rate: %d, SyncWord: %d, Power: %d, Preamble Length: %d, Max Packet Size: %d", 
             config.loraCs, config.loraRst, config.loraIrq, config.loraIo1, config.freq, config.bw, config.sf, config.cr, config.syncWord, config.power, config.preambleLength, config.max_packet_size);
            
            LoraMesher::getInstance().setConfig(config);
            break;
        case RuntimeConfigCommand::BackupCurrentConfig:
            backupConfig = LoraMesher::getInstance().getConfig();
            ESP_LOGI(RUNTIME_CONFIG_TAG, "Received BackupCurrentConfig message");
            ESP_LOGI(RUNTIME_CONFIG_TAG, 
             "Backed up config: CS: %d, RST: %d, IRQ: %d, IO1: %d, Freq: %f, Bandwidth: %f, Spreading Factor: %d, Coding Rate: %d, SyncWord: %d, Power: %d, Preamble Length: %d, Max Packet Size: %d", 
             backupConfig.loraCs, backupConfig.loraRst, backupConfig.loraIrq, backupConfig.loraIo1, backupConfig.freq, backupConfig.bw, backupConfig.sf, backupConfig.cr, backupConfig.syncWord, backupConfig.power, backupConfig.preambleLength, backupConfig.max_packet_size);
            break;
        case RuntimeConfigCommand::RestoreFromBackup:
            LoraMesher::getInstance().setConfig(backupConfig);
            ESP_LOGI(RUNTIME_CONFIG_TAG, "Received RestoreFromBackup message");
            ESP_LOGI(RUNTIME_CONFIG_TAG, 
             "New LoraMesher config: CS: %d, RST: %d, IRQ: %d, IO1: %d, Freq: %f, Bandwidth: %f, Spreading Factor: %d, Coding Rate: %d, SyncWord: %d, Power: %d, Preamble Length: %d, Max Packet Size: %d", 
             backupConfig.loraCs, backupConfig.loraRst, backupConfig.loraIrq, backupConfig.loraIo1, backupConfig.freq, backupConfig.bw, backupConfig.sf, backupConfig.cr, backupConfig.syncWord, backupConfig.power, backupConfig.preambleLength, backupConfig.max_packet_size);
            break;
        default:
            break;  
    }
}