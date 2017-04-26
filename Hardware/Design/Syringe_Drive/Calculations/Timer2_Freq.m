function [HzLow, HzHigh, clockTickTime, timeLow] = Timer2_Freq(prescaler)
% Timer2_Freq
%
% by: Steve Lammers
% Calculates the highest and lowest frequency for ATMEGA 328 Timer2
% based on a given prescaler value

clockSpeed = 16*1000*1000; %Hz
clockSpeedScaled = clockSpeed / prescaler; %Hz
clockTickTime = 1/clockSpeedScaled*1000*1000; % microseconds

HzHigh = clockSpeedScaled/2;
HzLow  = clockSpeedScaled/256/2; % for 8-bit

timeLow = clockTickTime*256*2/1000; % millisec for 8-bit