// .NAME example_histogram
// .SECTION Description
//  This example program shows a typical use of the vipl_histogram IP class on
//  an int image.  The input image (argv[1]) can be any scalar type (i.e.,
//  it should be mappable to int: [[un]signed] char, [unsigned]short, int),
//  and its histogram is calculated and written to stdout.
//  Uses vipl_histogram<section<ubyte,2>,section<int,2>,ubyte,int>.
//  The conversion between vil_image and the in-memory section<ubyte,2>
//  is done explicitly, pixel per pixel (because of possibly different types).
//
//  Note that it seems to be impossible with some compilers (notably
//  VisualC++ 5.0) to specify different types for input and output images!
//  That explains why I use section<int,2> and not section<ubyte,2>
//  as input image type.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 15 nov. 1997
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <../Image/ImageProcessingBasics/section.h>
#include <vil/vil_pixel.h>
#include <vipl/vipl_histogram.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_image.h>
#include <vcl_iostream.h>

#ifndef WIN32
typedef unsigned char ubyte;
#else
typedef int ubyte; // this is a hack!!!  See the Description.
#endif

int
main(int argc, char** argv) {
  if (argc < 2) { cerr << "Syntax: example_histogram file_in\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);

  section<ubyte,2> src(in.width(),in.height()); // in-memory 2D image
  section<int,2> dst(1,256);

  // set the input image:
  if (vil_pixel_type(in) != VIL_BYTE) { cerr << "Please use a ubyte image as input\n"; return 2; }
  in.get_section(src.buffer,0,0,in.width(),in.height());

  // The filter:
  vipl_histogram<section<ubyte,2>,section<int,2>,ubyte,int VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  {for (int i=0; i<256; ++i) if (src.buffer[i] != 0)
     cout << i << ": " << int(src.buffer[i]) << endl;
  }

  return 0;
}
