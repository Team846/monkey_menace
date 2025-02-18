#pragma once

#include "subsystems/hardware/generic/wrist_subsystem.h"

class CoralWristSubsystem : public WristSubsystem {
public:
  CoralWristSubsystem();

  WristTarget ZeroTarget() const override;

protected:
  void ExtendedSetup() override;
  std::pair<units::degree_t, bool> GetSensorPos() override;

private:
  wrist_pos_conv_t encoder_reduction = 45_tr / 1_tr;
  wrist_pos_conv_t encoder_to_subsystem_reduction = 40_tr / 16_tr;
};