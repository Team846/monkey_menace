#include "commands/teleop/coralgae_command.h"

CoralgaeCommand::CoralgaeCommand(RobotContainer &container)
    : frc846::robot::GenericCommand<RobotContainer, CoralgaeCommand>{
          container, "coralgae_command"} {
  AddRequirements({&container_.hyperstructure_});
}

void CoralgaeCommand::OnInit() {}

void CoralgaeCommand::Periodic() {
  HyperStructureTarget hyperstructure_target{};

  auto ci_readings = container_.control_input_.GetReadings();

  if (ci_readings.position_algal)
    hyperstructure_target.algalTarget.state = ci_readings.algal_state;

  if (ci_readings.position_coral)
    hyperstructure_target.coralTarget.state = ci_readings.coral_state;

  container_.hyperstructure_.SetTarget(hyperstructure_target);
}

void CoralgaeCommand::OnEnd(bool interrupted) {}

bool CoralgaeCommand::IsFinished() { return false; }
