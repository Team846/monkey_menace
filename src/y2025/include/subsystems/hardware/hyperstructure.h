#pragma once

#include "algal/algal_ss.h"
#include "coral/coral_ss.h"
#include "frc846/robot/GenericSubsystem.h"

struct HyperStructureTarget {
  CoralSSTarget coralTarget;
  AlgalSSTarget algalTarget;
};

struct HyperStructureReadings {};

class HyperStructure
    : public frc846::robot::GenericSubsystem<HyperStructureReadings,
          HyperStructureTarget> {
public:
  HyperStructure(AlgalSuperstructure* algae_ss, CoralSuperstructure* coral_ss);

  // void HandleCommand(AlgalSSTarget algae_target);
  // void HandleCommand(CoralSSTarget coral_target);

  void Setup() override;

  bool VerifyHardware() override;

  HyperStructureTarget ZeroTarget() const override { return {}; };
  HyperStructureReadings ReadFromHardware() override {
    return HyperStructureReadings{};
  };

  AlgalStates getLastAlgaeState();
  CoralStates getLastCoralState();

  void WriteToHardware(HyperStructureTarget target);

private:
  AlgalSuperstructure* algal_ss_;
  CoralSuperstructure* coral_ss_;

  bool is_algae_active;
  bool is_coral_active;

  HyperStructureTarget coralgaeTarget;
};