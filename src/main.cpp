#include <ctime>
#include <iostream>
#include <slurm/slurm.h>
#include <nlohmann/json.hpp>

/* Docs for slurm_job_info_t:
 *
 * https://github.com/SchedMD/slurm/blob/a1223ba9bff8ddc7ac30d18930412ef6b95caa28/slurm/slurm.h#L1759-L1914
*/

std::string job_state_to_string(uint32_t state) {
  switch (state) {
    case JOB_PENDING: return "JOB_PENDING";
    case JOB_RUNNING: return "JOB_RUNNING";
    case JOB_SUSPENDED: return "JOB_SUSPENDED";
    case JOB_COMPLETE: return "JOB_COMPLETE";
    case JOB_CANCELLED: return "JOB_CANCELLED";
    case JOB_FAILED: return "JOB_FAILED";
    case JOB_TIMEOUT: return "JOB_TIMEOUT";
    case JOB_NODE_FAIL: return "JOB_NODE_FAIL";
    case JOB_PREEMPTED: return "JOB_PREEMPTED";
    case JOB_BOOT_FAIL: return "JOB_BOOT_FAIL";
    case JOB_DEADLINE: return "JOB_DEADLINE";
    case JOB_OOM: return "JOB_OOM";
    case JOB_END: return "JOB_END";
    default: return "UNDEFINED"; // should be impossible
  };
}

std::string job_state_reason_to_string(job_state_reason state_reason){
  switch (state_reason){
    case WAIT_NO_REASON: return ""; /* not set or job not pending */
    case WAIT_PRIORITY: return "higher priority jobs exist";
    case WAIT_DEPENDENCY: return "dependent job has not completed";
    case WAIT_RESOURCES: return "required resources not available";
    case WAIT_PART_NODE_LIMIT: return "request exceeds partition node limit";
    case WAIT_PART_TIME_LIMIT: return "request exceeds partition time limit";
    case WAIT_PART_DOWN: return "requested partition is down";
    case WAIT_PART_INACTIVE: return "requested partition is inactive";
    case WAIT_HELD: return "job is held by administrator";
    case WAIT_TIME: return "job waiting for specific begin time";
    case WAIT_LICENSES: return "job is waiting for licenses";
    case WAIT_ASSOC_JOB_LIMIT: return "user/bank job limit reached";
    case WAIT_ASSOC_RESOURCE_LIMIT: return "user/bank resource limit reached";
    case WAIT_ASSOC_TIME_LIMIT: return "user/bank time limit reached";
    case WAIT_RESERVATION: return "reservation not available";
    case WAIT_NODE_NOT_AVAIL: return "required node is DOWN or DRAINED";
    case WAIT_HELD_USER: return "job is held by user";
    case WAIT_FRONT_END: return "Front end nodes are DOWN";
    case FAIL_DOWN_PARTITION: return "partition for job is DOWN";
    case FAIL_DOWN_NODE: return "some node in the allocation failed";
    case FAIL_BAD_CONSTRAINTS: return "constraints can not be satisfied";
    case FAIL_SYSTEM: return "slurm system failure";
    case FAIL_LAUNCH: return "unable to launch job";
    case FAIL_EXIT_CODE: return "exit code was non-zero";
    case FAIL_TIMEOUT: return "reached end of time limit";
    case FAIL_INACTIVE_LIMIT: return "reached slurm InactiveLimit";
    case FAIL_ACCOUNT: return "invalid account";
    case FAIL_QOS: return "invalid QOS";
    case WAIT_QOS_THRES: return "required QOS threshold has been breached";
    case WAIT_QOS_JOB_LIMIT: return "QOS job limit reached";
    case WAIT_QOS_RESOURCE_LIMIT: return "QOS resource limit reached";
    case WAIT_QOS_TIME_LIMIT: return "QOS time limit reached";
    case WAIT_CLEANING: return "If a job is requeued and it is still cleaning up from the last run.";
    case WAIT_PROLOG: return "Prolog is running";
    case WAIT_QOS: return "QOS not allowed";
    case WAIT_ACCOUNT: return "Account not allowed";
    case WAIT_DEP_INVALID: return "Dependency condition invalid or never satisfied          ";
    case WAIT_QOS_GRP_CPU: return "QOS GrpTRES exceeded (CPU)";
    case WAIT_QOS_GRP_CPU_MIN: return "QOS GrpTRESMins exceeded (CPU)";
    case WAIT_QOS_GRP_CPU_RUN_MIN: return "QOS GrpTRESRunMins exceeded (CPU)";
    case WAIT_QOS_GRP_JOB: return "QOS GrpJobs exceeded";
    case WAIT_QOS_GRP_MEM: return "QOS GrpTRES exceeded (Memory)";
    case WAIT_QOS_GRP_NODE: return "QOS GrpTRES exceeded (Node)";
    case WAIT_QOS_GRP_SUB_JOB: return "QOS GrpSubmitJobs exceeded";
    case WAIT_QOS_GRP_WALL: return "QOS GrpWall exceeded";
    case WAIT_QOS_MAX_CPU_PER_JOB: return "QOS MaxTRESPerJob exceeded (CPU)";
    case WAIT_QOS_MAX_CPU_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (CPU)";
    case WAIT_QOS_MAX_NODE_PER_JOB: return "QOS MaxTRESPerJob exceeded (Node)";
    case WAIT_QOS_MAX_WALL_PER_JOB: return "QOS MaxWallDurationPerJob exceeded";
    case WAIT_QOS_MAX_CPU_PER_USER: return "QOS MaxTRESPerUser exceeded (CPU)";
    case WAIT_QOS_MAX_JOB_PER_USER: return "QOS MaxJobsPerUser exceeded";
    case WAIT_QOS_MAX_NODE_PER_USER: return "QOS MaxTRESPerUser exceeded (Node)";
    case WAIT_QOS_MAX_SUB_JOB: return "QOS MaxSubmitJobsPerUser exceeded";
    case WAIT_QOS_MIN_CPU: return "QOS MinTRESPerJob not reached (CPU)";
    case WAIT_ASSOC_GRP_CPU: return "ASSOC GrpTRES exceeded (CPU)";
    case WAIT_ASSOC_GRP_CPU_MIN: return "ASSOC GrpTRESMins exceeded (CPU)";
    case WAIT_ASSOC_GRP_CPU_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (CPU)";
    case WAIT_ASSOC_GRP_JOB: return "ASSOC GrpJobs exceeded";
    case WAIT_ASSOC_GRP_MEM: return "ASSOC GrpTRES exceeded (Memory)";
    case WAIT_ASSOC_GRP_NODE: return "ASSOC GrpTRES exceeded (Node)";
    case WAIT_ASSOC_GRP_SUB_JOB: return "ASSOC GrpSubmitJobs exceeded";
    case WAIT_ASSOC_GRP_WALL: return "ASSOC GrpWall exceeded";
    case WAIT_ASSOC_MAX_JOBS: return "ASSOC MaxJobs exceeded";
    case WAIT_ASSOC_MAX_CPU_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (CPU)";
    case WAIT_ASSOC_MAX_CPU_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (CPU)";
    case WAIT_ASSOC_MAX_NODE_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (NODE)";
    case WAIT_ASSOC_MAX_WALL_PER_JOB: return "ASSOC MaxWallDurationPerJob exceeded";
    case WAIT_ASSOC_MAX_SUB_JOB: return "ASSOC MaxSubmitJobsPerUser exceeded";
    case WAIT_MAX_REQUEUE: return "MAX_BATCH_REQUEUE reached";
    case WAIT_ARRAY_TASK_LIMIT: return "job array running task limit";
    case WAIT_BURST_BUFFER_RESOURCE: return "Burst buffer resources";
    case WAIT_BURST_BUFFER_STAGING: return "Burst buffer file stage-in";
    case FAIL_BURST_BUFFER_OP: return "Burst buffer operation failure";
    case WAIT_POWER_NOT_AVAIL: return "not enough power available";
    case WAIT_POWER_RESERVED: return "job is waiting for available power because of power reservations";
    case WAIT_ASSOC_GRP_UNK: return "ASSOC GrpTRES exceeded (Unknown)";
    case WAIT_ASSOC_GRP_UNK_MIN: return "ASSOC GrpTRESMins exceeded (Unknown)";
    case WAIT_ASSOC_GRP_UNK_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (Unknown)";
    case WAIT_ASSOC_MAX_UNK_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (Unknown)";
    case WAIT_ASSOC_MAX_UNK_PER_NODE: return "ASSOC MaxTRESPerNode exceeded (Unknown)";
    case WAIT_ASSOC_MAX_UNK_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (Unknown)";
    case WAIT_ASSOC_MAX_CPU_PER_NODE: return "ASSOC MaxTRESPerNode exceeded (CPU)";
    case WAIT_ASSOC_GRP_MEM_MIN: return "ASSOC GrpTRESMins exceeded (Memory)";
    case WAIT_ASSOC_GRP_MEM_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (Memory)";
    case WAIT_ASSOC_MAX_MEM_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (Memory)";
    case WAIT_ASSOC_MAX_MEM_PER_NODE: return "ASSOC MaxTRESPerNode exceeded (CPU)";
    case WAIT_ASSOC_MAX_MEM_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (Memory)";
    case WAIT_ASSOC_GRP_NODE_MIN: return "ASSOC GrpTRESMins exceeded (Node)";
    case WAIT_ASSOC_GRP_NODE_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (Node)";
    case WAIT_ASSOC_MAX_NODE_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (Node)";
    case WAIT_ASSOC_GRP_ENERGY: return "ASSOC GrpTRES exceeded (Energy)";
    case WAIT_ASSOC_GRP_ENERGY_MIN: return "ASSOC GrpTRESMins exceeded (Energy)";
    case WAIT_ASSOC_GRP_ENERGY_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (Energy)";
    case WAIT_ASSOC_MAX_ENERGY_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (Energy)";
    case WAIT_ASSOC_MAX_ENERGY_PER_NODE: return "ASSOC MaxTRESPerNode exceeded (Energy)";
    case WAIT_ASSOC_MAX_ENERGY_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (Energy)";
    case WAIT_ASSOC_GRP_GRES: return "ASSOC GrpTRES exceeded (GRES)";
    case WAIT_ASSOC_GRP_GRES_MIN: return "ASSOC GrpTRESMins exceeded (GRES)";
    case WAIT_ASSOC_GRP_GRES_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (GRES)";
    case WAIT_ASSOC_MAX_GRES_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (GRES)";
    case WAIT_ASSOC_MAX_GRES_PER_NODE: return "ASSOC MaxTRESPerNode exceeded (GRES)";
    case WAIT_ASSOC_MAX_GRES_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (GRES)";
    case WAIT_ASSOC_GRP_LIC: return "ASSOC GrpTRES exceeded (license)";
    case WAIT_ASSOC_GRP_LIC_MIN: return "ASSOC GrpTRESMins exceeded (license)";
    case WAIT_ASSOC_GRP_LIC_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (license)";
    case WAIT_ASSOC_MAX_LIC_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (license)";
    case WAIT_ASSOC_MAX_LIC_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (license)";
    case WAIT_ASSOC_GRP_BB: return "ASSOC GrpTRES exceeded (burst buffer)";
    case WAIT_ASSOC_GRP_BB_MIN: return "ASSOC GrpTRESMins exceeded (burst buffer)";
    case WAIT_ASSOC_GRP_BB_RUN_MIN: return "ASSOC GrpTRESRunMins exceeded (burst buffer)";
    case WAIT_ASSOC_MAX_BB_PER_JOB: return "ASSOC MaxTRESPerJob exceeded (burst buffer)";
    case WAIT_ASSOC_MAX_BB_PER_NODE: return "ASSOC MaxTRESPerNode exceeded (burst buffer)";
    case WAIT_ASSOC_MAX_BB_MINS_PER_JOB: return "ASSOC MaxTRESMinsPerJob exceeded (burst buffer)";
    case WAIT_QOS_GRP_UNK: return "QOS GrpTRES exceeded (Unknown)";
    case WAIT_QOS_GRP_UNK_MIN: return "QOS GrpTRESMins exceeded (Unknown)";
    case WAIT_QOS_GRP_UNK_RUN_MIN: return "QOS GrpTRESRunMins exceeded (Unknown)";
    case WAIT_QOS_MAX_UNK_PER_JOB: return "QOS MaxTRESPerJob exceeded (Unknown)";
    case WAIT_QOS_MAX_UNK_PER_NODE: return "QOS MaxTRESPerNode exceeded (Unknown)";
    case WAIT_QOS_MAX_UNK_PER_USER: return "QOS MaxTRESPerUser exceeded (Unknown)";
    case WAIT_QOS_MAX_UNK_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (Unknown)";
    case WAIT_QOS_MIN_UNK: return "QOS MinTRESPerJob exceeded (Unknown)";
    case WAIT_QOS_MAX_CPU_PER_NODE: return "QOS MaxTRESPerNode exceeded (CPU)";
    case WAIT_QOS_GRP_MEM_MIN: return "QOS GrpTRESMins exceeded (Memory)";
    case WAIT_QOS_GRP_MEM_RUN_MIN: return "QOS GrpTRESRunMins exceeded (Memory)";
    case WAIT_QOS_MAX_MEM_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (Memory)";
    case WAIT_QOS_MAX_MEM_PER_JOB: return "QOS MaxTRESPerJob exceeded (CPU)";
    case WAIT_QOS_MAX_MEM_PER_NODE: return "QOS MaxTRESPerNode exceeded (MEM)";
    case WAIT_QOS_MAX_MEM_PER_USER: return "QOS MaxTRESPerUser exceeded (CPU)";
    case WAIT_QOS_MIN_MEM: return "QOS MinTRESPerJob not reached (Memory)";
    case WAIT_QOS_GRP_ENERGY: return "QOS GrpTRES exceeded (Energy)";
    case WAIT_QOS_GRP_ENERGY_MIN: return "QOS GrpTRESMins exceeded (Energy)";
    case WAIT_QOS_GRP_ENERGY_RUN_MIN: return "QOS GrpTRESRunMins exceeded (Energy)";
    case WAIT_QOS_MAX_ENERGY_PER_JOB: return "QOS MaxTRESPerJob exceeded (Energy)";
    case WAIT_QOS_MAX_ENERGY_PER_NODE: return "QOS MaxTRESPerNode exceeded (Energy)";
    case WAIT_QOS_MAX_ENERGY_PER_USER: return "QOS MaxTRESPerUser exceeded (Energy)";
    case WAIT_QOS_MAX_ENERGY_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (Energy)";
    case WAIT_QOS_MIN_ENERGY: return "QOS MinTRESPerJob not reached (Energy)";
    case WAIT_QOS_GRP_NODE_MIN: return "QOS GrpTRESMins exceeded (Node)";
    case WAIT_QOS_GRP_NODE_RUN_MIN: return "QOS GrpTRESRunMins exceeded (Node)";
    case WAIT_QOS_MAX_NODE_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (Node)";
    case WAIT_QOS_MIN_NODE: return "QOS MinTRESPerJob not reached (Node)";
    case WAIT_QOS_GRP_GRES: return "QOS GrpTRES exceeded (GRES)";
    case WAIT_QOS_GRP_GRES_MIN: return "QOS GrpTRESMins exceeded (GRES)";
    case WAIT_QOS_GRP_GRES_RUN_MIN: return "QOS GrpTRESRunMins exceeded (GRES)";
    case WAIT_QOS_MAX_GRES_PER_JOB: return "QOS MaxTRESPerJob exceeded (GRES)";
    case WAIT_QOS_MAX_GRES_PER_NODE: return "QOS MaxTRESPerNode exceeded (GRES)";
    case WAIT_QOS_MAX_GRES_PER_USER: return "QOS MaxTRESPerUser exceeded (GRES)";
    case WAIT_QOS_MAX_GRES_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (GRES)";
    case WAIT_QOS_MIN_GRES: return "QOS MinTRESPerJob not reached (CPU)";
    case WAIT_QOS_GRP_LIC: return "QOS GrpTRES exceeded (license)";
    case WAIT_QOS_GRP_LIC_MIN: return "QOS GrpTRESMins exceeded (license)";
    case WAIT_QOS_GRP_LIC_RUN_MIN: return "QOS GrpTRESRunMins exceeded (license)";
    case WAIT_QOS_MAX_LIC_PER_JOB: return "QOS MaxTRESPerJob exceeded (license)";
    case WAIT_QOS_MAX_LIC_PER_USER: return "QOS MaxTRESPerUser exceeded (license)";
    case WAIT_QOS_MAX_LIC_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (license)";
    case WAIT_QOS_MIN_LIC: return "QOS MinTRESPerJob not reached (license)";
    case WAIT_QOS_GRP_BB: return "QOS GrpTRES exceeded (burst buffer)";
    case WAIT_QOS_GRP_BB_MIN: return "QOS GrpTRESMins exceeded (burst buffer)";
    case WAIT_QOS_GRP_BB_RUN_MIN: return "QOS GrpTRESRunMins exceeded (burst buffer)";
    case WAIT_QOS_MAX_BB_PER_JOB: return "QOS MaxTRESPerJob exceeded (burst buffer)";
    case WAIT_QOS_MAX_BB_PER_NODE: return "QOS MaxTRESPerNode exceeded (burst buffer)";
    case WAIT_QOS_MAX_BB_PER_USER: return "QOS MaxTRESPerUser exceeded (burst buffer)";
    case WAIT_QOS_MAX_BB_MINS_PER_JOB: return "QOS MaxTRESMinsPerJob exceeded (burst buffer)";
    case WAIT_QOS_MIN_BB: return "QOS MinTRESPerJob not reached (burst buffer)";
    case FAIL_DEADLINE: return "reached deadline";
/* QOS MaxTRESPerAccount */
    case WAIT_QOS_MAX_BB_PER_ACCT: return "exceeded burst buffer";
    case WAIT_QOS_MAX_CPU_PER_ACCT: return "exceeded CPUs";
    case WAIT_QOS_MAX_ENERGY_PER_ACCT: return "exceeded Energy";
    case WAIT_QOS_MAX_GRES_PER_ACCT: return "exceeded GRES";
    case WAIT_QOS_MAX_NODE_PER_ACCT: return "exceeded Nodes";
    case WAIT_QOS_MAX_LIC_PER_ACCT: return "exceeded Licenses";
    case WAIT_QOS_MAX_MEM_PER_ACCT: return "exceeded Memory";
    case WAIT_QOS_MAX_UNK_PER_ACCT: return "exceeded Unknown";
/********************/
    case WAIT_QOS_MAX_JOB_PER_ACCT: return "QOS MaxJobPerAccount exceeded";
    case WAIT_QOS_MAX_SUB_JOB_PER_ACCT: return "QOS MaxJobSubmitSPerAccount exceeded";
    case WAIT_PART_CONFIG: return "Generic partition configuration reason";
    case WAIT_ACCOUNT_POLICY: return "Generic accounting policy reason";
    case WAIT_FED_JOB_LOCK: return "Can't get fed job lock";
    case FAIL_OOM: return "Exhausted memory";
    case WAIT_PN_MEM_LIMIT: return "MaxMemPer[CPU|Node] exceeded";
/* exceeded Billing TRES limits */
    case WAIT_ASSOC_GRP_BILLING: return "GrpTRES          ";
    case WAIT_ASSOC_GRP_BILLING_MIN: return "GrpTRESMins      ";
    case WAIT_ASSOC_GRP_BILLING_RUN_MIN: return "GrpTRESRunMins   ";
    case WAIT_ASSOC_MAX_BILLING_PER_JOB: return "MaxTRESPerJob    ";
    case WAIT_ASSOC_MAX_BILLING_PER_NODE: return "MaxTRESPerNode   ";
    case WAIT_ASSOC_MAX_BILLING_MINS_PER_JOB: return "MaxTRESMinsPerJob";
    case WAIT_QOS_GRP_BILLING: return "GrpTRES          ";
    case WAIT_QOS_GRP_BILLING_MIN: return "GrpTRESMins      ";
    case WAIT_QOS_GRP_BILLING_RUN_MIN: return "GrpTRESRunMins   ";
    case WAIT_QOS_MAX_BILLING_PER_JOB: return "MaxTRESPerJob    ";
    case WAIT_QOS_MAX_BILLING_PER_NODE: return "MaxTRESPerNode   ";
    case WAIT_QOS_MAX_BILLING_PER_USER: return "MaxTRESPerUser   ";
    case WAIT_QOS_MAX_BILLING_MINS_PER_JOB: return "MaxTRESMinsPerJob";
    case WAIT_QOS_MAX_BILLING_PER_ACCT: return "MaxTRESPerAcct   ";
    case WAIT_QOS_MIN_BILLING: return "MinTRESPerJob    ";
    case WAIT_RESV_DELETED: return "Reservation was deleted";
    default: {
      std::string out = "UNDEFINED: state_reason ";
      out += state_reason;
      return out;
    }
  }
}

void print_jobs_json(job_info_msg_t *jobs) {
  nlohmann::json json;

  for (size_t i = 0; i < jobs->record_count; i++) {
    auto job = jobs->job_array[i];

    /* Guaranteed keys */
    nlohmann::json jobson = {
      {"JobName", job.name},
      {"EndTime", job.end_time},
      {"JobId", job.job_id},
      {"NumTasks", job.num_tasks},
      {"JobState", job_state_to_string(job.job_state)},
      {"SubmitTime", job.submit_time},
      {"NumNodes", job.num_nodes},
      {"UserId", job.user_id},
      {"NumCPUs", job.num_cpus},
      {"WorkDir", job.work_dir},
      {"Partition", job.partition},
      {"command", job.command},
      {"EligibleTime", job.eligible_time},
      {"StartTime", job.start_time},
      {"CPUs/Task", job.cpus_per_task},
      {"ExitCode", job.exit_code},
    };

    /* Simple Optional keys, with `maybe(,)` helper */
    auto maybe = [&jobson] (auto &key, auto &value) {
      if (value) {
        jobson[key] = value;
      }
    };

    maybe("NodeList", job.nodes);
    maybe("Dependency", job.dependency);
    maybe("ArrayJobId", job.array_job_id);
    maybe("ArrayTaskId", job.array_task_id);
    maybe("BatchHost", job.batch_host);
    maybe("Licenses", job.licenses);
    maybe("Features", job.features);

    /* transformational or non-null-deciding Optional keys */
    if (job.state_reason) jobson["Reason"] = job_state_reason_to_string(static_cast<job_state_reason>(job.state_reason));
    if (!isinf(job.time_limit)) jobson["TimeLimit"] = job.time_limit;

    /* Compound value: Gres */
    if (job.gres_detail_cnt) {
      // FIXME: Doesn't work
      nlohmann::json greson;

      for (size_t j = 0; j < job.gres_detail_cnt; j++) {
        greson[j] = job.gres_detail_str[j];
      }

      jobson["Gres"] = greson;
    }

    // TODO: RunTime

    json[i] = jobson;
  }

  std::cout << json.dump(2) << std::endl;

  std::cout << "\nGot " << jobs->record_count << " Jobs, in " << json.dump().size() << "b" << std::endl;
}

void print_jobs(job_info_msg_t *job_info_msg_ptr) {
  std::cout << "Number of jobs: " << job_info_msg_ptr->record_count << "\n";
  std::cout << "Last update: " << job_info_msg_ptr->last_update << "\n";

  for (size_t i = 0; i < job_info_msg_ptr->record_count; i++) {
    std::cout << "  Job " << job_info_msg_ptr->job_array[i].job_id << " : "
              << job_info_msg_ptr->job_array[i].name << "\n";
    std::cout << "    Partition : " << job_info_msg_ptr->job_array[i].partition
              << "\n";
    std::cout << "    User      : " << job_info_msg_ptr->job_array[i].user_id
              << "\n";
    std::cout << "    Cores     : " << job_info_msg_ptr->job_array[i].num_cpus
              << "\n";
  }
}

int main() {
  long api_version = slurm_api_version();

  std::cout << "SLURM API version: " << api_version << "\n";

  const uint16_t show_flags = SHOW_ALL;
  time_t last_update(NULL);

  while (true) {
    job_info_msg_t *job_info_msg_ptr = NULL;
    int retval = slurm_load_jobs(last_update, &job_info_msg_ptr, show_flags);

    switch (retval){
      case 0:
        break;
      case 1900:
        // Data has not changed since time specified. All is well.
        break;
      default: {
        std::cerr << "slurm_load_jobs() failed: " << retval << "\n";

        auto slurm_errno = slurm_get_errno();
        std::cerr << "  slurm error: " << slurm_strerror(slurm_get_errno())
                  << "\n";
      }
    }

    if (job_info_msg_ptr != nullptr) {
    print_jobs_json(job_info_msg_ptr);

    last_update = job_info_msg_ptr->last_update;

    // free the job_info_msg_ptr
    slurm_free_job_info_msg(job_info_msg_ptr);
    job_info_msg_ptr = nullptr;
    }

    sleep(1); // seconds
  }

  std::cout << "done" << std::endl;

  return 0;
}
