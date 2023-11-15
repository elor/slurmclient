#include <iostream>
#include <slurm/slurm.h>

int main() {
  long api_version = slurm_api_version();

  std::cout << "SLURM API version: " << api_version << std::endl;

  return 0;
}
