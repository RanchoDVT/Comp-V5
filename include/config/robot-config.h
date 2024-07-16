extern vex::brain Brain;

extern vex::motor frontLeftMotor;
extern vex::motor frontRightMotor;

extern vex::motor rearLeftMotor;
extern vex::motor rearRightMotor;

extern vex::motor_group LeftDriveSmart;
extern vex::motor_group RightDriveSmart;

extern vex::smartdrive Drivetrain;

extern vex::controller primaryController;
extern vex::controller partnerController;

extern vex::inertial InertialGyro;

extern vex::bumper RearBumper;

extern vex::competition Competition;
/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexCodeInit(void);