#include <ctime>
#include <iostream>
#include <slurm/slurm.h>

int main() {
  long api_version = slurm_api_version();

  std::cout << "SLURM API version: " << api_version << std::endl;

  time_t never = time(nullptr);

  job_info_msg_t *job_info_msg_ptr = NULL;

  uint16_t show_flags = SHOW_ALL;

  int retval = slurm_load_jobs(never, &job_info_msg_ptr, show_flags);

  if (retval != 0) {
    std::cerr << "slurm_load_jobs() failed: " << retval << std::endl;
  }
  if (job_info_msg_ptr == nullptr) {
    std::cerr << "job_info_msg_ptr is null" << std::endl;
    return 1;
  }

  // print number of jobs, last update, and job ids

  std::cout << "Number of jobs: " << job_info_msg_ptr->record_count
            << std::endl;
  std::cout << "Last update: " << job_info_msg_ptr->last_update << std::endl;

  for (size_t i = 0; i < job_info_msg_ptr->record_count; i++) {
    std::cout << "  Job id: " << job_info_msg_ptr->job_array[i].job_id
              << std::endl;
  }

  // free the job_info_msg_ptr
  slurm_free_job_info_msg(job_info_msg_ptr);
  job_info_msg_ptr = nullptr;

  return 0;
}
