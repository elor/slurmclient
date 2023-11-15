#include "slurm/slurm_errno.h"
#include <ctime>
#include <iostream>
#include <slurm/slurm.h>

int main() {
  long api_version = slurm_api_version();

  std::cout << "SLURM API version: " << api_version << "\n";

  job_info_msg_t *job_info_msg_ptr = NULL;

  uint16_t show_flags = SHOW_ALL;

  int retval = slurm_load_jobs((time_t)NULL, &job_info_msg_ptr, show_flags);

  if (retval != 0) {
    std::cerr << "slurm_load_jobs() failed: " << retval << "\n";

    std::cerr << "  slurm error: " << slurm_strerror(slurm_get_errno()) << "\n";
  }

  if (job_info_msg_ptr == nullptr) {
    std::cerr << "job_info_msg_ptr is null"
              << "\n";
    return 1;
  }

  // print number of jobs, last update, and job ids

  std::cout << "Number of jobs: " << job_info_msg_ptr->record_count << "\n";
  std::cout << "Last update: " << job_info_msg_ptr->last_update << "\n";

  for (size_t i = 0; i < job_info_msg_ptr->record_count; i++) {
    std::cout << "  Job " << job_info_msg_ptr->job_array[i].job_id
	      << " : " << job_info_msg_ptr->job_array[i].name
              << "\n";
    std::cout << "    Partition : " << job_info_msg_ptr->job_array[i].partition
              << "\n";
    std::cout << "    User      : " << job_info_msg_ptr->job_array[i].user_id
	      << "\n";
    std::cout << "    Cores     : " << job_info_msg_ptr->job_array[i].num_cpus
              << "\n";
  }

  // free the job_info_msg_ptr
  slurm_free_job_info_msg(job_info_msg_ptr);
  job_info_msg_ptr = nullptr;

  std::cout << "done" << std::endl;

  return 0;
}
