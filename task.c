#include "gastask.h"

unsigned	n_tasks;
task_t	tasks[MAX_TASKS];

void
get_task_utilpower(unsigned no_task, unsigned char mem_type, unsigned char cpufreq_type, double *putil, double *ppower_cpu, double *ppower_mem)
{
	task_t	*task = tasks + no_task; 
	// mem_t	*mem = mems + mem_type; 
	double swap_ratio = swapRatio[mem_type]; //
	cpufreq_t	*cpufreq = cpufreqs + cpufreq_type; 

	double	wcet_scaled_cpu = 1 / cpufreq->wcet_scale; 
	// double	wcet_scaled_mem = 1 / mem->wcet_scale; //
	double	wcet_scaled_dram = 1 / mems[0].wcet_scale; //
	double  wcet_scaled_storage = 1 / mems[1].wcet_scale; //
	double 	wcet_scaled_mem; //
	double 	mem_power_active; //
	double  mem_power_stat;
	double	cpu_power_unit;
	double	wcet_scaled;
	double  command_time = 0.4;

//	wcet_scaled_mem = ( wcet_scaled_dram * (1 - swap_ratio) + (wcet_scaled_dram + 2 * wcet_scaled_storage) * swap_ratio); //
	wcet_scaled_mem = wcet_scaled_dram + swap_ratio * command_time; // swapratio 0.5 일때 접근 시간 1.2되도록...
	// wcet_scaled = task->wcet * wcet_scaled_cpu * wcet_scaled_mem; //비례하는 느낌으로..
	wcet_scaled = task->wcet * max(wcet_scaled_cpu, wcet_scaled_mem); 
//	printf("%f %f %d\n", swap_ratio, wcet_scaled, task->wcet);
	if (wcet_scaled >= task->period) {
		FATAL(3, "task[%u]: scaled wcet exceeds task period: %lf > %u", task->no, wcet_scaled, task->period);
	}
	*putil = wcet_scaled / task->period; //utilization

	cpu_power_unit = (cpufreq->power_active * wcet_scaled_cpu + cpufreq->power_idle * wcet_scaled_mem) / (wcet_scaled_cpu + wcet_scaled_mem);
	*ppower_cpu = cpu_power_unit * wcet_scaled / task->period; //Ecpu

//	printf("%f %f\n", swap_ratio, mem_power_active);
	// *ppower_mem = 
	// task->memreq * (task->mem_active_ratio * mem_power_active + (1 - task->mem_active_ratio) * mems[0].power_idle) * wcet_scaled / task->period 
	// + task->memreq * mems[0].power_idle * (1 - wcet_scaled / task->period); //Emem
	mem_power_active = task->memreq * (task->mem_active_ratio * ((1-swap_ratio) * mems[0].power_active + swap_ratio * mems[1].power_active)) * wcet_scaled/task->period;
	mem_power_stat = task->memreq * (1-task->mem_active_ratio) * (1-swap_ratio) * mems[0].power_idle * wcet_scaled / task->period
						+ task->memreq * (1-swap_ratio) * mems[0].power_idle * (1 - wcet_scaled / task->period);
	// *ppower_mem = task->memreq * (task->mem_active_ratio * ((1-swap_ratio) * mems[0].power_active + swap_ratio * mems[1].power_active)
	// 			+ (1-task->mem_active_ratio) * (1-swap_ratio) * mems[0].power_idle) * wcet_scaled / task->period
	// 			+ task->memreq * (1-swap_ratio) * mems[0].power_idle * (1 - wcet_scaled / task->period);
	*ppower_mem = mem_power_active + mem_power_stat;
}

unsigned
get_task_memreq(unsigned no_task)
{
	task_t	*task = tasks + no_task;

	return task->memreq;
}

void
add_task(unsigned wcet, unsigned period, unsigned memreq, double mem_active_ratio)
{
	task_t	*task;

	task = tasks + n_tasks;
	task->wcet = wcet;
	task->period = period;
	task->memreq = memreq;
	task->mem_active_ratio = mem_active_ratio;

	n_tasks++;
	task->no = n_tasks;
}
