#pragma once

#include "calculators/gpd.h"
#include "frc846/robot/GenericCommand.h"
#include "subsystems/robot_container.h"

class DriveCommand
    : public frc846::robot::GenericCommand<RobotContainer, DriveCommand> {
public:
  DriveCommand(RobotContainer& container);

  void OnInit() override;

  void Periodic() override;

  void OnEnd(bool interrupted) override;

  bool IsFinished() override;

private:
  GPDCalculator gpd;
  std::shared_ptr<nt::NetworkTable> gpdTable =
      nt::NetworkTableInstance::GetDefault().GetTable("GPDCam1");
};