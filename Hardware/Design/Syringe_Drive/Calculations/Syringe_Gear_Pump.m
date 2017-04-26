% Syringe_Gear_Pump
%
% by: Steve Lammers
% This program calculates the gear ratio needed to dispense
% a given diameter bead through a syringe needle using a
% stepper motor

% Define CONSTANTS
NEEDLE_GA          = 32;      % Gauge
NEEDLE_DIAM        = 0.11;    % mm
SYRINGE_SIZE       = 1;       % mL
SYRINGE_ID         = 9;%4.78;    % mm
STEPPER_GEAR_TEETH = 15;      % number of teeth on stepper spur gear
STEPPER_GEAR_PD    = 9.6;     % pitch diameter of stepper spur gear (mm)
STEPPER_ANGLE      = 1.8;     % degrees (200 steps)
STEPPER_MICROSTEPS = 16;       % microsteps/full step
STEPPER_ANG_VEL    = 10;      % stepper angular velocity rad/s
LINEAR_VELOCITY    = 100;     % gantery speed mm/s
                              % feedrate for X direction, can be set
                              % with G1 X100 F6000 where F is in mm/min
RACK_GEAR_TEETH    = 12;      % number of teeth on the drive grear
RACK_GEAR_DIAM     = .5*25.4; % mm PD
GEAR_RATIO         = 110;     % 110:1
    
% Anonomyous functions
deg2rad = @(deg) deg * pi()/180;
rad2deg = @(rad) rad * 180/pi();

% Calculate DEFAULT_AXIS_STEPS_PER_UNIT for Lulzbot
stepsPerUnit = Default_Axis_Steps_Per_Unit();

% Calculate dispense rate needed
% Start by saying the dispense rate is equal to the LINEAR_VELOCITY * area
dispenseRate = LINEAR_VELOCITY * pi()/4 * NEEDLE_DIAM^2; % mm^3 / sec

% Calculate plunger displacement for given dispense rate
% dispenseRate / syringe area = mm/s plunger velocity
plungerVelocity = dispenseRate/(pi()/4*SYRINGE_ID^2); % mm/s

% Calculate the angle that the rack drive gear must turn
% in order to dispense at plungerVelocity
rackAngVel = plungerVelocity / (pi() * RACK_GEAR_DIAM)*(2*pi());

% Choose a revolutions / second
%stepperRPS = 1/12; %ge.25;
%stepperLinearVel = stepperRPS * STEPPER_GEAR_PD * pi();
stepperAngVel = deg2rad(47);%47 * pi()/180;%pi()/2;

% Calculate Gear Ratio Needed
%gearRatio = stepperLinearVel/plungerVelocity;
%gearRatio = stepperAngVel/rackAngVel;

% Calculate the stepper angular displacement in deg
stepperAng = rad2deg(GEAR_RATIO * rackAngVel);

% Calculate # of steps taken
numSteps = stepperAng * 200 / 360 * STEPPER_MICROSTEPS;

% Calculate micro seconds per step
stepTime = 1/numSteps*1000*1000;