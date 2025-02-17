#include "frc846/robot/calculators/AprilTagCalculator.h"

#include "frc846/robot/GenericRobot.h"
#include "frc846/wpilib/time.h"

namespace frc846::robot::calculators {

ATCalculatorOutput AprilTagCalculator::calculate(ATCalculatorInput input) {
  ATCalculatorOutput output;

  double totalTagWeight = 0;
  double variance = 0;
  for (int i = 0; i < constants_.cams; i++) {
    units::second_t delay =
        frc846::wpilib::CurrentFPGATime() -
        units::microsecond_t(
            constants_.april_tables.at(i)->GetEntry("tl").GetLastChange());
    units::second_t tl =
        units::second_t(constants_.april_tables.at(i)->GetNumber("tl", -1)) +
        delay;
    if (delay > 3.5 * frc846::robot::GenericRobot::kPeriod) { continue; }

    std::vector<double> tx_nums =
        constants_.april_tables.at(i)->GetNumberArray("tx", {});
    std::vector<double> distances_num =
        constants_.april_tables.at(i)->GetNumberArray("distances", {});
    std::vector<units::degree_t> tx;
    std::vector<units::inch_t> distances;
    for (double tx_num : tx_nums) {
      tx.push_back(units::degree_t(tx_num));
    };
    for (double distance_num : distances_num) {
      distances.push_back(units::inch_t(distance_num));
    };

    std::vector<double> tags =
        constants_.april_tables.at(i)->GetNumberArray("tags", {});
    units::degree_t bearingAtCapture =
        input.pose.bearing -
        input.angular_velocity *
            (tl + input.bearing_latency);  // TODO: fix bearing latency, accl?

    // CASE 1: Triangulate with 2 tags
    if (tags.size() >= 2 && tags.size() == distances.size() &&
        tags.size() == tx.size()) {
      auto loc_tag_1 = constants_.tag_locations.find(tags[0]);
      auto loc_tag_2 = constants_.tag_locations.find(tags[1]);

      frc846::math::Vector2D tag_1_pos = {
          loc_tag_1->second.x_pos, loc_tag_1->second.y_pos};
      frc846::math::Vector2D tag_2_pos = {
          loc_tag_2->second.x_pos, loc_tag_2->second.y_pos};

      auto delta_tag_pos = tag_1_pos - tag_2_pos;
      auto sum_slope_vecs = frc846::math::Vector2D{1_in, tx[0], true} +
                            frc846::math::Vector2D{1_in, tx[1], true};

      double t = -delta_tag_pos[0] / sum_slope_vecs[0];

      frc846::math::Vector2D est_pos =
          tag_1_pos - (frc846::math::Vector2D{1_in, tx[0], true} * t);

      frc846::math::Vector2D velComp = {
          (input.pose.velocity[0] + input.old_pose.velocity[0]) / 2 *
              (tl + input.fudge_latency),
          (input.pose.velocity[1] + input.old_pose.velocity[1]) / 2 *
              (tl + input.fudge_latency)};

      output.pos += (est_pos + velComp) * 1.2;
      variance +=
          1 / std::max(
                  (input.triangularVarianceCoeff *
                      std::sqrt(distances.at(0).to<double>()) *
                      std::pow(
                          1 + input.pose.velocity.magnitude().to<double>(), 2) *
                      std::pow(1 + input.angular_velocity.to<double>(), 2)),
                  0.0000000001);
      totalTagWeight += 1.2;

    }
    // CASE 2: Single tag estimate
    else if (tags.size() == 1 && distances.size() == 1 && tx.size() == 1) {
      if (constants_.tag_locations.contains(tags[0])) {
        frc846::math::Vector2D velComp = {
            (input.pose.velocity[0] + input.old_pose.velocity[0]) / 2 *
                (tl + input.fudge_latency),
            (input.pose.velocity[1] + input.old_pose.velocity[1]) / 2 *
                (tl + input.fudge_latency)};
        output.pos += (getPos(bearingAtCapture, tx.at(0), distances.at(0),
                           tags.at(0), i) +
                          velComp) *
                      (48) / distances.at(0).to<double>();
        variance +=
            1 /
            std::max(
                (input.aprilVarianceCoeff *
                    std::sqrt(distances.at(0).to<double>()) *
                    std::pow(
                        1 + input.pose.velocity.magnitude().to<double>(), 2) *
                    std::pow(1 + input.angular_velocity.to<double>(), 2)),
                0.0000000001);
        totalTagWeight += (48) / distances.at(0).to<double>();
      }
    }
  }

  if (totalTagWeight != 0) {
    output.variance = 1 / variance;
    output.pos = output.pos / totalTagWeight;
    correction = output.pos - input.pose.position;
    return output;
  } else {
    return {input.pose.position + correction, -1};
  }
}

frc846::math::VectorND<units::inch, 2> AprilTagCalculator::getPos(
    units::degree_t bearing, units::degree_t theta, units::inch_t distance,
    int tag, int camera) {
  frc846::math::VectorND<units::inch, 2> cam_to_tag{
      distance * units::math::sin(theta),
      distance * units::math::cos(theta),
  };
  frc846::math::VectorND<units::inch, 2> center_to_cam{
      constants_.camera_x_offsets[camera],
      constants_.camera_y_offsets[camera],
  };

  frc846::math::VectorND<units::inch, 2> local_tag_pos =
      center_to_cam + cam_to_tag;
  local_tag_pos = local_tag_pos.rotate(bearing);

  return {
      constants_.tag_locations[tag].x_pos - local_tag_pos[0],
      constants_.tag_locations[tag].y_pos - local_tag_pos[1],
  };
}
}