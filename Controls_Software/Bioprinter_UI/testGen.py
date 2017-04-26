from g_code_generator import gen_gcode_cube
import unittest

class TestGcodeGenerator(unittest.TestCase):
  def test_filename(self):
    with self.assertRaises(TypeError):
      gen_gcode_cube(gcode_ilename = "foo.gcode")

if __name__=='__main__':
  unittest.main()
