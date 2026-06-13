#pragma once

#include <Arduino.h>

#include "message/dataMessage.h"

#pragma pack(1)

enum RuntimeConfigCommand : uint8_t {
    ModifyConfig = 0,
    BackupCurrentConfig = 1,
    RestoreFromBackup = 2,
};

class ModifyConfigMessage : public DataMessageGeneric {
public:
    RuntimeConfigCommand runtimeConfigCommand;

    // LoRa pins
    uint8_t loraCs;
    uint8_t loraIrq;
    uint8_t loraRst; 
    uint8_t loraIo1; 

    // LoRa configuration
    float freq; 
    float bw; 
    uint8_t sf; 
    uint8_t cr; 
    uint8_t syncWord; 
    int8_t power; 
    uint16_t preambleLength; 
    size_t max_packet_size;

    void serialize(JsonObject& doc) {
        // Create a data object
        JsonObject dataObj = doc.createNestedObject("data");

        // Add the data to the data object
        serializeModifyConfigSerialize(dataObj);
    }

    void serializeModifyConfigSerialize(JsonObject& doc) {
        ESP_LOGI("Pre Data Serialize", "Message not for me");
        // Call the base class serialize function
        ((DataMessageGeneric*)(this))->serialize(doc);

        doc["message_type"] = "ModifyConfig";
    }

    void deserialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*)(this))->deserialize(doc);
        runtimeConfigCommand = doc["runtimeConfigCommand"];
        loraCs = doc["loraCs"];
        loraIrq = doc["loraIrq"];
        loraRst = doc["loraRst"];
        loraIo1 = doc["loraIo1"];

        freq = doc["freq"];
        bw = doc["bw"];
        sf = doc["sf"];
        cr = doc["cr"];
        syncWord = doc["syncWord"];
        power = doc["power"];
        preambleLength = doc["preambleLength"];
        max_packet_size = doc["max_packet_size"];
    }
};

class BackupCurrentConfigMessage : public DataMessageGeneric {
public:
    RuntimeConfigCommand runtimeConfigCommand;

    void serialize(JsonObject& doc) {
        // Create a data object
        JsonObject dataObj = doc.createNestedObject("data");

        // Add the data to the data object
        serializeBackupCurrentConfigMessage(dataObj);
    }

    void serializeBackupCurrentConfigMessage(JsonObject& doc) {
        ESP_LOGI("Pre Data Serialize", "Message not for me");
        // Call the base class serialize function
        ((DataMessageGeneric*)(this))->serialize(doc);

        doc["message_type"] = "BackupCurrentConfig";
    }

    void deserialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*)(this))->deserialize(doc);
        runtimeConfigCommand = doc["runtimeConfigCommand"];
    }
};

class RestoreFromBackupMessage : public DataMessageGeneric {
public:
    RuntimeConfigCommand runtimeConfigCommand;

    void serialize(JsonObject& doc) {
        // Create a data object
        JsonObject dataObj = doc.createNestedObject("data");

        // Add the data to the data object
        serializeRestoreFromBakcupSerialize(dataObj);
    }

    void serializeRestoreFromBakcupSerialize(JsonObject& doc) {
        ESP_LOGI("Pre Data Serialize", "Message not for me");
        // Call the base class serialize function
        ((DataMessageGeneric*)(this))->serialize(doc);

        doc["message_type"] = "RestoreFromBackup";
    }

    void deserialize(JsonObject& doc) {
        // Call the base class serialize function
        ((DataMessageGeneric*)(this))->deserialize(doc);
        runtimeConfigCommand = doc["runtimeConfigCommand"];
    }
};

class RuntimeConfigCommandMessage : public DataMessageGeneric {
public:
    RuntimeConfigCommand runtimeConfigCommand;
    uint8_t payload[];

    void serialize(JsonObject& doc) {
        switch (runtimeConfigCommand) {
            case RuntimeConfigCommand::ModifyConfig:
                ((ModifyConfigMessage*)(this))->serialize(doc);
                break;
            case RuntimeConfigCommand::BackupCurrentConfig:
                ((BackupCurrentConfigMessage*)(this))->serialize(doc);
                break;
            case RuntimeConfigCommand::RestoreFromBackup:
                ((RestoreFromBackupMessage*)(this))->serialize(doc);
                break;                
        }
    }

    void deserialize(JsonObject& doc) {
        switch ((RuntimeConfigCommand)doc["runtimeConfigCommand"]) {
            case RuntimeConfigCommand::ModifyConfig:
                ((ModifyConfigMessage*)(this))->deserialize(doc);
                break;
            case RuntimeConfigCommand::BackupCurrentConfig:
                ((BackupCurrentConfigMessage*)(this))->deserialize(doc);
                break;
            case RuntimeConfigCommand::RestoreFromBackup:
                ((RestoreFromBackupMessage*)(this))->deserialize(doc);
                break;
        }
        // Add the derived class data to the JSON object
        runtimeConfigCommand = doc["runtimeConfigCommand"];
    }
};


#pragma pack()