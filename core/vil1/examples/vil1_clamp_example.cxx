#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_clamp.h>
#include <vil/vil_image.h>
#include <vil/vil_image_as.h>
#include <vil/vil_pixel.h>

int main (int argc, char** argv) {

  if (argc != 5) {
    vcl_cerr << __FILE__ " : low_clamp high_clamp in_image out_image" << vcl_endl;
    abort();
  }

  double low = atof(argv[1]);
  double high = atof(argv[2]);

  vil_image in = vil_load(argv[3]);
  if (vil_pixel_type(in) == VIL_BYTE)
    vil_save(vil_image_as_byte(vil_clamp(in, low, high)), argv[4], "pnm");
  else if (vil_pixel_type(in) == VIL_RGB_BYTE)
    vil_save(vil_image_as_rgb_byte(vil_clamp(in, low, high)), argv[4], "pnm");
  
}
