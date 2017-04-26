# WRITE FILE
# -------------------------------------------

# GLOBALS
PRINT_SPEED = 450 # mm/s
XY_SPEED    = 1000
Z_SPEED     = 300
UV_DELAY_TIME = 2 # Bake time for UV light
X_CENTER_POINT = 58
Y_CENTER_POINT = 58
Z_DISH_OFFSET = 20
X_POS = 0   # Global X position 
Y_POS = 0   # Global Y position
Z_POS = 0   # Global Z position
X_PREV_POS = 0
Y_PREV_POS = 0
Z_PREV_POS = 0
UV_X_CENTER_POINT = 82
UV_Y_CENTER_POINT = 58
UV_Z_CENTER_POINT = 10

def gen_gcode_cube( gcode_filename="testPrint_01.gcode", vert_offset=2.0, horz_offset=2.0, z_offset=.15, 
                    number_rows_vert=6, number_rows_horz=6, number_rows_z=4 ):
  '''
  Function to generate the g-code to print a cube given:
    vert_offset: vert offset between horz lines in mm 
    horz_offset: horz offset between vert lines in mm 
    z_offset: offset between planes
    number_rows_vert: number of vert lines 
    number_rows_horz: number of horz lines 
    number_rows_z: number of layers
  '''
  # Create and/or overwrite a file
  f = open(gcode_filename, 'w')

  # DEFINITIONS
  # ------------------------------------------
  '''
  vert_offset = 2.0 #1.0 #.2+.337
  horz_offset = 2.0 #1.0 #.2+.337
  z_offset = .15#.2#.337

  number_rows_vert = 6 #10
  number_rows_horz = 6 #10
  number_rows_z = 4 #round(10/z_offset)

  '''

  cube_horz_len = number_rows_horz*horz_offset
  cube_vert_len = number_rows_vert*vert_offset

  print("cube_horz_len: " + str(cube_horz_len))
  print("cube_vert_len: " + str(cube_vert_len))
  print("filename: ", gcode_filename)

  # Erase everyting in the existing file
  f.truncate()

  # Write the preamble
  # Move to Origin (Home)
  f.write("G28\n")
  # Set to tool #10 - bioprinter printhead 
  f.write("T10\n")
  # Set speed to 4500mm/min
  f.write("G1 F300\n")
  # Set positioning to relative
  #f.write("G91\n")
  # Set positioning to absolute
  f.write("G90\n")
  # move gantry up to clear build surface
  move_z(f, Z_DISH_OFFSET, 0)
  # Move to center of build plate 
  move_xyz(f, X_CENTER_POINT, Y_CENTER_POINT, 0, 0)
  move_xyz(f, -cube_horz_len/2, -(cube_vert_len/2 - 3), 0, 0)
  # move gantry back to build surface
  move_z(f, -Z_DISH_OFFSET, 0)

  # Begin extrusion to get everything flowing
  pre_game(f)

  # ---------------------------------------------
  # Print cube: 
  #       meander_vert(direction, start_side)
  #       meander_horz(direction, start_side)
  # ---------------------------------------------
  ind_rows_z = 1
  while ind_rows_z <= number_rows_z:
    # meander up start left-bottom and end left-top
    f.write("; Stack Number: " + str(ind_rows_z) + "\n")
    meander_vert( f, 'up', 'left', number_rows_vert, cube_horz_len, vert_offset)

    # move gantry up
    move_z(f, z_offset)
    f.write("\n")
    # bake the gel with UV light
    UV_bake(f, UV_DELAY_TIME)
    # increment row number and check if done
    ind_rows_z = ind_rows_z + 1
    if ind_rows_z > number_rows_z:
      break

    # meander right start left-top end right-top
    f.write("; Stack Number: " + str(ind_rows_z) + "\n")
    meander_horz( f, 'right', 'top', number_rows_horz, cube_vert_len, horz_offset)

    # move gantry up
    move_z(f, z_offset)
    f.write("\n")
    # bake the gel with UV light
    UV_bake(f, UV_DELAY_TIME)
    # increment row number and check if done
    ind_rows_z = ind_rows_z + 1
    if ind_rows_z > number_rows_z:
      break

    # meander up start right-top and end right-bottom
    f.write("; Stack Number: " + str(ind_rows_z) + "\n")
    meander_vert( f, 'down', 'right', number_rows_vert, cube_horz_len, vert_offset)

    # move gantry up
    move_z(f, z_offset)
    f.write("\n")
    # bake the gel with UV light
    UV_bake(f, UV_DELAY_TIME)
    # increment row number and check if done
    ind_rows_z = ind_rows_z + 1
    if ind_rows_z > number_rows_z:
      break

    # meander right start right-bottom end left-bottom
    f.write("; Stack Number: " + str(ind_rows_z) + "\n")
    meander_horz( f, 'left', 'bottom', number_rows_horz, cube_vert_len, horz_offset)

    # move gantry up
    move_z(f, z_offset)
    f.write("\n")
    # bake the gel with UV light
    UV_bake(f, UV_DELAY_TIME)
    # increment row number and check if done
    ind_rows_z = ind_rows_z + 1
    if ind_rows_z > number_rows_z:
      break



  # Set to positioning absolute
  f.write("G90\n")
  # Set to tool #0
  f.write("T0\n")
  # Set speed to 4500mm/min
  #f.write("G1 F4500\n")
  # Set to positioning relative
  #f.write("G91\n")
  # Move to Origin (Home)
  f.write("G28\n")
  # Program End
  f.write("M2\n")


# FUNCTION DEFINITIONS
#--------------------------------------------
def move_x( f, dist, extrude=1):
  global X_POS
  global XY_SPEED
  X_POS += dist
  if extrude:
    f.write("G1 X" + str(X_POS) + " " + "F" + str(PRINT_SPEED) + " E1\n")
  else: 
    f.write("G1 X" + str(X_POS) + " " + "F" + str(XY_SPEED) + "\n")
  return

def move_y( f, dist, extrude=1):
  global Y_POS
  global XY_SPEED
  Y_POS += dist
  if extrude:
    f.write("G1 Y" + str(Y_POS) + " " + "F" + str(PRINT_SPEED) + " E1\n")
  else:
    f.write("G1 Y" + str(Y_POS) + " " + "F" + str(XY_SPEED) + "\n")
  return

def move_z(f, dist, extrude=1):
  global Z_POS
  global Z_SPEED
  Z_POS += dist
  if extrude:
    f.write("G1 Z" + str(Z_POS) + " " + "F" + str(PRINT_SPEED) +" E1\n")
  else:
    f.write("G1 Z" + str(Z_POS) + " " + "F" + str(Z_SPEED) +"\n")
  return

def move_xyz(f, x_dist, y_dist, z_dist, extrude=1):
  global X_POS
  global Y_POS
  global Z_POS
  global PRINT_SPEED
  global XY_SPEED
  global Z_SPEED
  X_POS += x_dist
  Y_POS += y_dist
  Z_POS += z_dist
  if extrude:
    f.write("G1 X" + str(X_POS) + " " +\
               "Y" + str(Y_POS) + " " + \
               "Z" + str(Z_POS) + " " + \
               "F" + str(PRINT_SPEED) + " E1" + "\n")
  else:
    f.write("G1 X" + str(X_POS) + " " + \
               "Y" + str(Y_POS) + " " + \
               "Z" + str(Z_POS) + " " + \
               "F" + str(XY_SPEED) + "\n")
  return

def move_to_xyz_abs(f, x_abs, y_abs, z_abs, extrude=1, move_speed=0):
  global X_POS
  global Y_POS
  global Z_POS
  global PRINT_SPEED
  global XY_SPEED
  global Z_SPEED
  X_POS = x_abs
  Y_POS = y_abs
  Z_POS = z_abs
  if extrude:
    if move_speed != 0:
      f.write("G1 X" + str(X_POS) + " " +\
                 "Y" + str(Y_POS) + " " + \
                 "Z" + str(Z_POS) + " " + \
                 "F" + str(move_speed) + " E1" + "\n")
    else:
      f.write("G1 X" + str(X_POS) + " " +\
                 "Y" + str(Y_POS) + " " + \
                 "Z" + str(Z_POS) + " " + \
                 "F" + str(PRINT_SPEED) + " E1" + "\n")

  else:
    if move_speed != 0:
      f.write("G1 X" + str(X_POS) + " " + \
                 "Y" + str(Y_POS) + " " + \
                 "Z" + str(Z_POS) + " " + \
                 "F" + str(move_speed) + "\n")
    else:
      f.write("G1 X" + str(X_POS) + " " + \
                 "Y" + str(Y_POS) + " " + \
                 "Z" + str(Z_POS) + " " + \
                 "F" + str(XY_SPEED) + "\n")
  return

def UV_bake(f, delay_time):
  global X_POS
  global Y_POS
  global Z_POS
  global X_PREV_POS
  global Y_PREV_POS
  global Z_PREV_POS

  X_PREV_POS = X_POS
  Y_PREV_POS = Y_POS
  Z_PREV_POS = Z_POS
  
  # Move UV_LED to a position over the print
  #  First reposition the printhead to clear the print dish
  #  Then move UV light in XY direction and finally move to 
  #  UV light illumination distance 
  #  Remember - Z only moves must go slower b/c firmware will default?
  move_to_xyz_abs(f, X_POS, Y_POS, Z_DISH_OFFSET, 0, 300)
  move_to_xyz_abs(f, UV_X_CENTER_POINT, UV_Y_CENTER_POINT, Z_DISH_OFFSET, 0)
  move_to_xyz_abs(f, UV_X_CENTER_POINT, UV_Y_CENTER_POINT, UV_Z_CENTER_POINT, 0, 300)
  f.write("G4 P100\n");                     # Short dwell needed to ensure that movement 
                                            #  buffer finishes before excuting UV_LED on
                                            #  command is sent.
  f.write("G5\n")                           # Turn on UV_LED
  f.write("G4 S"+ str(delay_time) +"\n")    # Dwell for delay_time(seconds)
  f.write("G6\n")                           # Turn off UV_LED
  # Return printhead to print
  move_to_xyz_abs(f, X_PREV_POS, Y_PREV_POS, Z_DISH_OFFSET, 0)
  move_to_xyz_abs(f, X_PREV_POS, Y_PREV_POS, Z_PREV_POS, 0, 300)
  f.write("\n")

def pre_game(f):
  move_x(f, 20)
  move_y(f, 1)
  move_x(f, -20)
  move_y(f, 2)
  f.write("\n")
  

def meander_vert( f, vert_direction, start_side, number_rows_vert, cube_horz_len, vert_offset):
  row_ind=1
  while row_ind <= number_rows_vert:
    
    # Print a line
    if start_side == 'left':
      move_x(f, cube_horz_len)
    elif start_side == 'right':
      move_x(f, -cube_horz_len)
    else:
      raise NameError("Start Side must be 'left' or 'right'")
    
    # Step up/down by offset to next line
    if vert_direction == 'up':
      move_y(f, vert_offset)
    elif vert_direction == 'down':
      move_y(f, -vert_offset)
    else:
      raise NameError("Dirction must be 'up' or 'down'")

    # Increment row index
    row_ind += 1
    
    # Print return line
    if start_side == 'left':
      move_x(f, -cube_horz_len)
    elif start_side == 'right':
      move_x(f, cube_horz_len)
    else:
      raise NameError("Start Side must be 'left' or 'right'")
    

    # Step up/down by offset to next line, unless last line reached
    if row_ind == number_rows_vert:
      f.write("\n")
      break
    if vert_direction == 'up':
      move_y(f, vert_offset)
    elif vert_direction == 'down':
      move_y(f, -vert_offset)
    else:
      raise NameError("Dirction must be 'up' or 'down'")
    
    # Increment row index
    row_ind += 1
    f.write("\n")
    
def meander_horz( f, horz_direction, start_side, number_rows_horz, cube_vert_len, horz_offset):
  row_ind=1
  while row_ind <= number_rows_horz:
    
    # Print a line
    if start_side == 'top':
      move_y(f, -cube_vert_len)
    elif start_side == 'bottom':
      move_y(f, cube_vert_len)
    else:
      raise NameError("Start Side must be 'top' or 'bottom'")
    
    # Step up/down by offset to next line
    if horz_direction == 'right':
      move_x(f, horz_offset)
    elif horz_direction == 'left':
      move_x(f, -horz_offset)
    else:
      raise NameError("Dirction must be 'right' or 'left'")
    
    # Increment row index
    row_ind += 1

    # Print return line
    if start_side == 'top':
      move_y(f, cube_vert_len)
    elif start_side == 'bottom':
      move_y(f, -cube_vert_len)
    else:
      raise NameError("Start Side must be 'top' or 'bottom'")
    

    # Step up/down by offset to next line, unless last line reached
    if row_ind == number_rows_horz:
      f.write("\n")
      break
    if horz_direction == 'right':
      move_x(f, horz_offset)
    elif horz_direction == 'left':
      move_x(f, -horz_offset)
    else:
      raise NameError("Dirction must be 'right' or 'left'")
    
    # Increment row index
    row_ind += 1
    f.write("\n")
    
