#include "bvpl_edge3d_kernel_factory.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vnl/vnl_quaternion.h>

// Default Constructor
bvpl_edge3d_kernel_factory::bvpl_edge3d_kernel_factory()
{
  height_=0;
  width_=0;
  length_=0;
  angular_resolution_ = 0;
  canonical_rotation_axis_[0] = 0.0; canonical_rotation_axis_[1] = 0.0; canonical_rotation_axis_[2] = 0.0;
  canonical_parallel_axis_[0] = 0.0; canonical_parallel_axis_[1] = 0.0; canonical_parallel_axis_[2] = 0.0;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_parallel_axis_;
  angle_ = 0.0f;
}

//: Constructs an edge of dimensions height*width*length. The canonical form of this edge is along the y - axis
// + + 0 - -
// + + 0 - -
// + + 0 - -
// + + 0 - -
bvpl_edge3d_kernel_factory::bvpl_edge3d_kernel_factory(unsigned length, unsigned width, unsigned height)
{
  //set dimensions
  length_=length;
  height_=height;
  width_=width;


  //Determine angular resolition based on size of kernel
  //If this was 2D, then the angular resolution would be 180/(2l -2) (Recusive Binary Dilation... Desikachari Nadadur)
  angular_resolution_=0;
  //set canonical axis to x-axis
  canonical_rotation_axis_[0] = 1.0; canonical_rotation_axis_[1] = 0.0; canonical_rotation_axis_[2] = 0.0;

  canonical_parallel_axis_[0] = 0.0; canonical_parallel_axis_[1] = 1.0; canonical_parallel_axis_[2] = 0.0;
  angle_ = 0.0f;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_paralell_axis_;
  create_canonical();
}

void bvpl_edge3d_kernel_factory::create_canonical()
{
  if ( !(height_ % 2))
  {
    vcl_cerr << "Warning, height of kernel is even. It has been increased by one\n";
    height_++;
  }
  if (!(width_ % 2))
  {
    vcl_cerr << "Warning, width of kernel is even. It has been increased by one\n";
    width_++;
  }
  //The size of the kernel is limited. If widht or height of the kernel is too large,
  //the user should subsample the image/grid
  if (height_ > max_size_)
  {
    vcl_cerr<< "Warning, height of kernel is too large. It has been set to is max value of" << max_size_ << vcl_endl;
    height_ = max_size_;
  }
  if (width_ > max_size_)
  {
    vcl_cerr<< "Warning, weight of kernel is too large. It  has been set to is max value of" << max_size_ << vcl_endl;
    width_ = max_size_;
  }

  //create a basic centered edge with 1, 0, -1, if width is odd.  The edge is creates on the x-z plane
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1
  // 1 1 0 -1 -1

  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  int min_x= -1*(length_/2);
  int max_x =(length_/2);
  int min_y= -1*(width_/2);
  int max_y =(width_/2);
  int min_z= -1*(height_/2);
  int max_z =(height_/2);

  int z = 0;
  for (int x=min_x; x<= max_x; x++)
  {
    for (int y= min_y; y<= max_y; y++)
    {
      for (int z= min_z; z<= max_z; z++)
      {
        if (x < 0)
          canonical_kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(float(x),float(y),float(z)), dispatch(-1)));
        else if (x >  0)
          canonical_kernel_.push_back(vcl_pair<point_3d,dispatch>(point_3d(float(x),float(y),float(z)), dispatch(1)));
      }
    }
  }
  //set the dimension of the 3-d grid
  max3d_.set(max_x,max_y,max_z);
  min3d_.set(min_x,min_y,min_z);

  //set the current kernel
  kernel_ = canonical_kernel_;

  return;
}


/******************Batch Methods ***********************/

//: Creates a vector of kernels with azimuthal(\phi) and elevation(\theta) resolution equal to pi/4.
//  This uses spherical coordinates where \theta \in  [0,\pi) and \phi \in [0,2\pi)
bvpl_kernel_vector_sptr bvpl_edge3d_kernel_factory::create_kernel_vector()
{
  bvpl_kernel_vector_sptr kernels = new bvpl_kernel_vector();
  float theta_res = float(vnl_math::pi_over_4);
  float phi_res = float(vnl_math::pi_over_4);
  vnl_vector_fixed<float, 3> axis;

  float theta = 0.0f,
        phi = 0.0f;

  //polar theta=0,pi
  axis[0] =0.0f;
  axis[1] =0.0f;
  axis[2] =1.0f;
  this->set_rotation_axis(axis);
  kernels->kernels_.push_back(vcl_make_pair(axis, new bvpl_kernel(this->create())));

  axis[0] =0.0f;
  axis[1] =0.0f;
  axis[2] =-1.0f;
  this->set_rotation_axis(axis);
  kernels->kernels_.push_back(vcl_make_pair(axis, new bvpl_kernel(this->create())));

  // theta=pi/4,pi/2,3pi/4
  for (theta=vnl_math::pi_over_4;theta <= 3*float(vnl_math::pi_over_4); )
  {
    for (phi=0.0;phi<2*vnl_math::pi;phi+=phi_res)
    {
      axis[0] = vcl_cos(phi) * vcl_sin(theta);
      axis[1] = vcl_sin(phi) * vcl_sin(theta);
      axis[2] = vcl_cos(theta);
      this->set_rotation_axis(axis);
      kernels->kernels_.push_back(vcl_make_pair(axis, new bvpl_kernel(this->create())));
    }
    theta +=theta_res;
  }
  return kernels;
}

//: Creates a vector of kernels according to given  azimuthal and elevation resolutio, and angle of rotation= angular_resolution_
bvpl_kernel_vector_sptr bvpl_edge3d_kernel_factory::create_kernel_vector(float pi, float phi)
{
  //to be implemented
  return 0;
}

//: Creates a vector of kernels  according to given azimuthal, levation resolutio and angle_res
bvpl_kernel_vector_sptr bvpl_edge3d_kernel_factory::create_kernel_vector(float pi, float phi, float angular_res)
{
  //to be impemented
  return 0;
}
