#include "control_triggers.h"

#include <frc2/command/InstantCommand.h>
#include <frc2/command/WaitCommand.h>
#include <frc2/command/button/Trigger.h>

#include "commands/teleop/lock_gpd_command.h"
#include "commands/teleop/lock_to_reef_command.h"
#include "frc846/robot/swerve/aim_command.h"
#include "frc846/robot/swerve/drive_to_point_command.h"
#include "reef.h"

void ControlTriggerInitializer::InitTeleopTriggers(RobotContainer& container) {
  frc2::Trigger drivetrain_zero_bearing_trigger{[&] {
    return container.control_input_.GetReadings().zero_bearing;
  }};
  drivetrain_zero_bearing_trigger.WhileTrue(frc2::InstantCommand([&] {
    container.drivetrain_.ZeroBearing();
  }).ToPtr());

  // FAKE, TODO: remove

  frc2::Trigger test_move_10_ft_trigger{[&] {
    return container.control_input_.GetReadings().test_move_10_ft;
  }};
  test_move_10_ft_trigger.WhileTrue(
      frc846::robot::swerve::DriveToPointCommand{&container.drivetrain_,
          {{0_ft, 9_ft}, 0_deg, 0_fps}, 15_fps, 35_fps_sq, 15_fps_sq}
          .ToPtr());

  frc2::Trigger test_bearing_pid_trigger{[&] {
    return container.control_input_.GetReadings().test_bearing_pid;
  }};
  test_bearing_pid_trigger.WhileTrue(
      frc846::robot::swerve::AimCommand{&container.drivetrain_, 0_deg}.ToPtr());

  // END FAKE

  frc2::Trigger{[&] {
    return container.control_input_.GetReadings().lock_left_reef;
  }}.WhileTrue(LockToReefCommand{container, true}.ToPtr());
  frc2::Trigger{[&] {
    return container.control_input_.GetReadings().lock_right_reef;
  }}.WhileTrue(LockToReefCommand{container, false}.ToPtr());

  frc2::Trigger{[&] {
    return container.control_input_.GetReadings().targeting_algae &&
           container.GPD_.GetReadings().gamepieces.size() != 0U;
  }}.OnTrue(LockGPDCommand{container}.Until([&] {
    return !container.control_input_.GetReadings().targeting_algae;
  }));
}
