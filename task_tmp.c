void
get_task_utilpower(unsigned no_task, unsigned char mem_type, unsigned char cpufreq_type, double *putil, double *ppower_cpu, double *ppower_mem)
{
	task_t	*task = tasks + no_task; 
//  double swap_ratio = swapRatio[mem_type] 
	cpufreq_t	*cpufreq = cpufreqs + cpufreq_type; 

	double	wcet_scaled_cpu = 1 / cpufreq->wcet_scale; 
//	double	wcet_scaled_dram = 1 / mems[0]->wcet_scale; 
//  double  wcet_scaled_storage = 1 / mems[1]->wcet_scale;
	double	cpu_power_unit;
	double	wcet_scaled;

	wcet_scaled = task->wcet * wcet_scaled_cpu * wcet_scaled_mem;
//  wcet_scaled_mem = ( wcet_scaled_dram * (1 - swap_ratio) + (wcet_scaled_dram + 2 * wcet_scaled_storage) * swap_ratio)
//  wcet_scaled = task->wcet * wcet_scaled_cpu * wcet_scaled_mem;
	if (wcet_scaled >= task->period) {
		FATAL(3, "task[%u]: scaled wcet exceeds task period: %lf > %u", task->no, wcet_scaled, task->period);
	}
	*putil = wcet_scaled / task->period; //f(ti) / pi, utilization

	cpu_power_unit = (cpufreq->power_active * wcet_scaled_cpu + cpufreq->power_idle * wcet_scaled_mem) / (wcet_scaled_cpu + wcet_scaled_mem);
	*ppower_cpu = cpu_power_unit * wcet_scaled / task->period; //Ecpu 

	*ppower_mem = task->memreq * (task->mem_active_ratio * mem->power_active + (1 - task->mem_active_ratio) * mem->power_idle) * wcet_scaled / task->period +
		task->memreq * mem->power_idle * (1 - wcet_scaled / task->period); //Emem

//  mem_power_active = mems[0]->power_active * (1-swap_ratio) + (mems[0]->power_active + mems[1]->power_active) * swap_ratio
//  mem_power_idle = mems[0]->power_idle * (1-swap_ratio) + (mems[0]->power_idle + mems[1]->power_idle) * swap_ratio --> XXXXXXXX

//  *ppower_mem = 
//   task->memreq * (task->mem_active_ratio * mem_power_active + (1 - task->mem_active_ratio) * mem_power_idle) * wcet_scaled / task->period +
//	task->memreq * swap_ratio * mems[0]->power_idle * (1 - wcet_scaled / task->period);
}