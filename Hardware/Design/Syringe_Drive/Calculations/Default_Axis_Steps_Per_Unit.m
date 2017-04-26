function stepArray = Default_Axis_Steps_Per_Unit()
% Default_Axis_Steps_Per_Unit
%
% by: Steve Lammers
% Calculates the DEFAULT_AXIS_STEPS_PER_UNIT for the
% stepper motors in the printer. Returns steps/mm for
% [X,Y,Z,E]
% FROM: https://www.matterhackers.com/news/3d-printer-firmware-settings-stepper-motor-configuration
% Also useful: http://forums.reprap.org/read.php?146,153414

% Define CONSTANTS
STEPS_PER_REVOLUTION_X = 3200; % 1.8degree stepper with 1/16 
STEPS_PER_REVOLUTION_Y = 3200; % microstep Polou driver
STEPS_PER_REVOLUTION_Z = 3200; 
STEPS_PER_REVOLUTION_E = 3200; 

IDLER_TEETH_X  = 16; % Number of idler teeth on stepper gear 
IDLER_TEETH_Y  = 16;
BELT_PITCH_X   = 2;  % mm / tooth
BELT_PITCH_Y   = 2;
PITCH_OF_Z_ROD = 2;  % mm / turn

EXTRUDER_GEAR_RATIO  = 47/9; % gear ratio of herringbone gears
PINCH_WHEEL_DIAMETER = 8;    % mm, diameter of hobbed bolt

axis_steps_per_unit_x = STEPS_PER_REVOLUTION_X / IDLER_TEETH_X ...
                        / BELT_PITCH_X;
axis_steps_per_unit_y = STEPS_PER_REVOLUTION_Y / IDLER_TEETH_Y ...
                        / BELT_PITCH_Y;
axis_steps_per_unit_z = STEPS_PER_REVOLUTION_Z / PITCH_OF_Z_ROD;
axis_steps_per_unit_e = STEPS_PER_REVOLUTION_E * EXTRUDER_GEAR_RATIO ...
                        / (PINCH_WHEEL_DIAMETER * pi());
                    

stepArray = [axis_steps_per_unit_x, ...
             axis_steps_per_unit_y, ...
             axis_steps_per_unit_z, ...
             axis_steps_per_unit_e];

