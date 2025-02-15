#include "FunkyRobot.h"

#include <cameraserver/CameraServer.h>
#include <frc/DSControlWord.h>
#include <frc/Filesystem.h>
#include <frc/RobotController.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc2/command/WaitCommand.h>
#include <frc2/command/button/Trigger.h>
#include <hal/Notifier.h>

#include "autos/auton_seqs.h"
#include "calculators/AntiTippingCalculator.h"
#include "commands/general/algal_position_command.h"
#include "commands/general/coral_position_command.h"
#include "commands/general/dinosaur_climber.h"
#include "commands/teleop/algal_command.h"
#include "commands/teleop/climber_command.h"
#include "commands/teleop/coral_command.h"
#include "commands/teleop/drive_command.h"
#include "control_triggers.h"
#include "field.h"
#include "frc846/wpilib/NTAction.h"
#include "rsighandler.h"
#include "subsystems/hardware/leds_logic.h"
#include <ctre/phoenix6/TalonFX.hpp>

#include <iostream>

FunkyRobot::FunkyRobot() : GenericRobot{&container_} {}

void FunkyRobot::OnInitialize() {
  Field::Setup();

  // for (auto x : Field::getAllAutoData()) {
  //   Log("Adding Auto: {}", x.name + "_red");
  //   AddAuto(x.name + "_red", new GenericAuto{container_, x, false});
  //   Log("Adding Auto: {}", x.name + "_blue");
  //   AddAuto(x.name + "_blue", new GenericAuto{container_, x, true});
  // }

  ADD_AUTO_VARIANTS(ThreePieceAuto, "3PC");
  ADD_AUTO_VARIANTS(OnePieceAndNetAuto, "1PCN");
  ADD_AUTO_VARIANTS(LeaveAuto, "LEAVE");

  // // Add dashboard buttons
  frc::SmartDashboard::PutData("set_cancoder_offsets",
      new frc846::wpilib::NTAction(
          [this] { container_.drivetrain_.SetCANCoderOffsets(); }));
  frc::SmartDashboard::PutData(
      "zero_bearing", new frc846::wpilib::NTAction(
                          [this] { container_.drivetrain_.ZeroBearing(); }));

  frc::SmartDashboard::PutData("zero_odometry",
      new frc846::wpilib::NTAction(
          [this] { container_.drivetrain_.SetPosition({0_in, 0_in}); }));
}

void FunkyRobot::OnDisable() {}

void FunkyRobot::InitTeleop() {
  container_.drivetrain_.SetDefaultCommand(DriveCommand{container_});

  container_.coral_ss_.SetDefaultCommand(CoralCommand{container_});
  container_.algal_ss_.SetDefaultCommand(AlgalCommand{container_});
  container_.climber_.SetDefaultCommand(ClimberCommand{container_});

  ControlTriggerInitializer::InitTeleopTriggers(container_);
}

void FunkyRobot::OnPeriodic() {
  if (!home_switch_.Get()) {
    container_.algal_ss_.elevator.HomeSubsystem(
        robot_constants::elevator::elevator_hall_effect);
    container_.coral_ss_.telescope.HomeSubsystem(
        robot_constants::telescope::telescope_hall_effect);
  }

  LEDsLogic::UpdateLEDs(&container_);

  AntiTippingCalculator::SetTelescopeHeight(
      container_.coral_ss_.telescope.GetReadings().position);
  AntiTippingCalculator::SetElevatorHeight(
      container_.algal_ss_.elevator.GetReadings().position);

  auto cg = AntiTippingCalculator::CalculateRobotCG();
  Graph("robot_cg_x", cg[0]);
  Graph("robot_cg_y", cg[1]);
  Graph("robot_cg_z", cg[2]);

  if (counter>0){
    counter--;
    if (counter==0){
      jurassicOrch.Play();
      std::cout<<"hola"<<std::endl;
    }
  }

  std::cout<<jurassicOrch.IsPlaying()<<std::endl;
  // jurassicOrch.Play();
}

void FunkyRobot::InitTest() {
  counter=10;
  // container_.drivetrain_.SetDefaultCommand(DriveCommand{container_});
  // container_.climber_.SetDefaultCommand(DinosaurClimberCommand{container_});

  // frc2::Trigger start_dinosaur_a([] { return true; });
  // start_dinosaur_a.WhileTrue(frc2::SequentialCommandGroup{
  //     AlgalPositionCommand{container_, kAlgae_DINOSAUR_A, true},
  //     frc2::WaitCommand{0.5_s},
  //     AlgalPositionCommand{container_, kAlgae_DINOSAUR_B, true},
  //     frc2::WaitCommand{0.5_s}}.Repeatedly());

  // frc2::Trigger start_dinosaur_c([] { return true; });
  // start_dinosaur_c.WhileTrue(frc2::SequentialCommandGroup{
  //     CoralPositionCommand{container_, kCoral_DINOSAUR_A, true},
  //     frc2::WaitCommand{0.5_s},
  //     CoralPositionCommand{container_, kCoral_DINOSAUR_B, true},
  //     frc2::WaitCommand{0.5_s}}.Repeatedly());

  frc2::Trigger start_chirp([] { return true; });

  std::cout<<"hola como muy bien estas"<<std::endl;

  
  std::cout<<"hola como estas"<<std::endl;
  jurassicOrch.LoadMusic("jurassic.chrp");
      // ctre::phoenix6::Orchestra("jurassic.chrp");
  // std::array<frc846::robot::swerve::SwerveModuleSubsystem*, 4> swerves =
  //     container_.drivetrain_.GetModules();
  // for (frc846::robot::swerve::SwerveModuleSubsystem* swerve : swerves) {
  //   swerve->GetDrive().AddToOrchestra(jurassicOrch);
  //   swerve->GetSteer().AddToOrchestra(jurassicOrch);
  // }
  ctre::phoenix6::hardware::TalonFX talon = ctre::phoenix6::hardware::TalonFX(2);
  jurassicOrch.AddInstrument(talon);
  // jurassicOrch.Play();
}

#ifndef RUNNING_FRC_TESTS
int main() {
  if (frc::RobotBase::IsSimulation()) configureSignalHandlers();
  return frc::StartRobot<FunkyRobot>();
}
#endif