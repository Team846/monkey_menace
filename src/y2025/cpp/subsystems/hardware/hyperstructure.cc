#include "subsystems/hardware/hyperstructure.h"

HyperStructure::HyperStructure(
    AlgalSuperstructure* algae_ss, CoralSuperstructure* coral_ss)
    : frc846::robot::GenericSubsystem<HyperStructureReadings,
          HyperStructureTarget>{"hyperstructure"},
      algal_ss_{algae_ss},
      coral_ss_{coral_ss} {}

void HyperStructure::Setup() {}

bool HyperStructure::VerifyHardware() {
  return coral_ss_->VerifyHardware() && algal_ss_->VerifyHardware();
}

void HyperStructure::WriteToHardware(HyperStructureTarget target) {
  if (target.algalTarget.state == AlgalStates::kAlgae_L2Pick &&
      target.coralTarget.state == CoralStates::kCoral_ScoreL2) {
    if (algal_ss_->last_state == kAlgae_L2Pick) {
      algal_ss_->SetTarget(target.algalTarget);
    } else {
      coral_ss_->SetTarget(target.coralTarget);
    }
  } else if (target.algalTarget.state == AlgalStates::kAlgae_L2Pick &&
             target.coralTarget.state == CoralStates::kCoral_ScoreL3) {
    if (algal_ss_->last_state == kAlgae_L2Pick) {
      algal_ss_->SetTarget(target.algalTarget);
    } else {
      coral_ss_->SetTarget(target.coralTarget);
    }
  } else if (target.algalTarget.state == AlgalStates::kAlgae_L3Pick &&
             target.coralTarget.state == CoralStates::kCoral_ScoreL3) {
    if (algal_ss_->last_state == kAlgae_L3Pick) {
      algal_ss_->SetTarget(target.algalTarget);
    } else {
      coral_ss_->SetTarget(target.coralTarget);
    }
  } else {
    algal_ss_->SetTarget(target.algalTarget);
    coral_ss_->SetTarget(target.coralTarget);
  }
}