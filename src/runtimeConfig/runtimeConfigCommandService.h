#pragma once

#include "Arduino.h"

#include "commands/commandService.h"

class RuntimeConfigCommandService : public CommandService {
public:
    RuntimeConfigCommandService();
};