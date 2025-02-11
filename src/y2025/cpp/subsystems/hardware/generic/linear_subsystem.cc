#include "subsystems/hardware/generic/linear_subsystem.h"

#include "subsystems/SubsystemHelper.h"

LinearSubsystem::LinearSubsystem(std::string name,
    frc846::control::base::MotorMonkeyType mmtype,
    frc846::control::config::MotorConstructionParameters motor_configs_,
    linear_pos_conv_t conversion, units::inch_t hall_effect_loc_)
    : frc846::robot::GenericSubsystem<LinearSubsystemReadings,
          LinearSubsystemTarget>(name),
      linear_esc_(mmtype, GetCurrentConfig(motor_configs_)),
      hall_effect_loc_(hall_effect_loc_) {
  REGISTER_PIDF_CONFIG(0.0, 0.0, 0.0, 0.0);
  // REGISTER_SOFTLIMIT_CONFIG("limits", true, 30_in, 0_in, 30_in, 0_in, 0.3);

  linear_esc_helper_.SetConversion(conversion);

  linear_esc_helper_.bind(&linear_esc_);
}

frc846::control::config::MotorConstructionParameters
LinearSubsystem::GetCurrentConfig(
    frc846::control::config::MotorConstructionParameters original_config) {
  frc846::control::config::MotorConstructionParameters modifiedConfig =
      original_config;
  REGISTER_MOTOR_CONFIG(40_A, 30_A);
  modifiedConfig.motor_current_limit =
      GetPreferenceValue_unit_type<units::ampere_t>(
          "motor_configs/current_limit");
  modifiedConfig.smart_current_limit =
      GetPreferenceValue_unit_type<units::ampere_t>(
          "motor_configs/smart_current_limit");
  return modifiedConfig;
}

void LinearSubsystem::Setup() {
  linear_esc_.Setup();

  linear_esc_.EnableStatusFrames(
      {frc846::control::config::StatusFrame::kPositionFrame,
          frc846::control::config::StatusFrame::kVelocityFrame,
          frc846::control::config::StatusFrame::kFaultFrame});

  // linear_esc_helper_.SetSoftLimits(GET_SOFTLIMITS("limits", units::inch_t));
  // linear_esc_helper_.SetControllerSoftLimits(
  //     GET_SOFTLIMITS("limits", units::inch_t));

  linear_esc_.ConfigForwardLimitSwitch(
      true, frc846::control::base::LimitSwitchDefaultState::kNormallyOff);

  const auto [sensor_pos, is_valid] = GetSensorPos();
  if (is_valid) { linear_esc_helper_.SetPosition(sensor_pos); }

  ExtendedSetup();
}

bool LinearSubsystem::VerifyHardware() {
  bool ok = true;
  FRC846_VERIFY(linear_esc_.VerifyConnected(), ok, "Could not verify esc");
  return ok;
}

LinearSubsystemReadings LinearSubsystem::ReadFromHardware() {
  LinearSubsystemReadings readings;
  readings.position = linear_esc_helper_.GetPosition();

  linear_esc_.SetLoad(1_Nm);

  Graph("readings/position", readings.position);

  const auto [sensor_pos, is_valid] = GetSensorPos();
  if (is_valid) { linear_esc_helper_.SetPosition(sensor_pos); }

  Graph("readings/sensor_pos", sensor_pos);
  Graph("readings/sensor_pos_valid", is_valid);

  bool forward_limit = linear_esc_.GetForwardLimitSwitchState();

  Graph("readings/homing_sensor", forward_limit);

  if (forward_limit && !is_homed_) {
    is_homed_ = true;
    linear_esc_helper_.SetPosition(hall_effect_loc_);
  }

  return readings;
}

void LinearSubsystem::WriteToHardware(LinearSubsystemTarget target) {
  Graph("target/position", target.position);
  linear_esc_.SetGains(GET_PIDF_GAINS());

  if (!is_homed_) {
    linear_esc_helper_.WriteDC(0.1);
  } else {
    linear_esc_helper_.WritePosition(target.position);
  }
}