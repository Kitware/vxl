#ifndef vil_to_vbl_array_h_
#define vil_to_vbl_array_h_

#include <vil/vil_image.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_iostream.h>

inline vbl_array_2d<unsigned char> vil_to_vbl_array(vil_image const& im)
{
  if (im.planes() > 1)
    { vcl_cerr << "vil_to_vbl_array: Error: cannot handle >1 plane\n";
      return vbl_array_2d<unsigned char>(0,0); }
  if (im.component_format() != VIL_COMPONENT_FORMAT_UNSIGNED_INT
      || im.components() != 1 || im.bits_per_component() != 8)
    { vcl_cerr << "vil_to_vbl_array: Error: can only handle UBYTE images\n";
      return vbl_array_2d<unsigned char>(0,0); }

  int width = im.width();
  int height = im.height();

  vbl_array_2d<unsigned char> imo(height,width);
  unsigned char* buf = imo.begin();
  im.get_section(buf, 0, 0, width, height);
  return imo;
}

#endif // vil_to_vbl_array_h_
