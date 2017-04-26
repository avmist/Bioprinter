function mL = mL_Per_Foot(id)
% mL_Per_Foot
%
% by: Steve Lammers
% Calculates mL/ft given a for a tube with a given id

mL = 1/4*pi()*((id*2.54)^2)*(12*2.54);