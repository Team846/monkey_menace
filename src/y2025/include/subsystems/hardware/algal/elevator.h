#pragma once

#include "subsystems/hardware/generic/linear_subsystem.h"

class ElevatorSubsystem : public LinearSubsystem {
public:
  ElevatorSubsystem();

protected:
  void ExtendedSetup() override;
  std::pair<units::inch_t, bool> GetSensorPos() override;

private:
};