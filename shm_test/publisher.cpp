#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object shdmem{open_or_create, "Gain", read_write};
  shdmem.truncate(1024);
  mapped_region region{shdmem, read_write};
  double *gain = static_cast<double*>(region.get_address());
  *gain = 0.1;
}
