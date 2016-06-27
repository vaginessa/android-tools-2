/*
 ============================================================================
 Name        : cpu_load.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUF_SIZE 64
#define DEFAULT_INTERVAL 1000

struct freq_info {
	unsigned freq;
};

struct cpu_info {
	long unsigned utime, ntime, stime, itime, iowtime, irqtime, sirqtime;
	struct freq_info cpu_freqs;
};

#define die(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE);}
#define MAX_CPU_NUMBER 8
static struct cpu_info old_total_cpu, new_total_cpu;
static struct cpu_info *old_cpus, *new_cpus;
static int cpu_count;

//读取CPU核心个数
static int get_cpu_count_from_file(char *filename) {
	FILE *file;
	char line[MAX_BUF_SIZE];
	int cpu_count;
	file = fopen(filename, "r");
	if (!file)
		die("could not open %s\n", filename);
	if (!fgets(line, MAX_BUF_SIZE, file))
		die("Could not get  %s contents\n", filename);
	fclose(file);
	if (strcmp(line, "0\n") == 0)
		return 1;
	if (1 == sscanf(line, "0-%d\n", &cpu_count))
		return cpu_count + 1;
	die("Unexpected input in file %s (%s).\n", filename, line);
	return -1;
}

//获取系统CPU核心数
static int get_cpu_count() {
	int cpu_count = get_cpu_count_from_file("/sys/devices/system/cpu/online");
	return cpu_count;
}


//获取指定CPU的频率信息
int get_cpu_freq(int index) {
	char file_name[256];
	char buf[32];
	snprintf(file_name, 256,
			"/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", index);
	int fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		perror("fail to open file: ");
		return -1;
	}
	read(fd, buf, 32);
	close(fd);
	return atoi(buf);
}
//读取/proc/stat文件信息
static void read_stats() {
	FILE * file;
	char scanline[MAX_BUF_SIZE];
	char line[MAX_BUF_SIZE];
	int i;
	file = fopen("/proc/stat", "r");
	if (!file)
		die("Could not open /proc/stat.\n");
	fscanf(file, "cpu %lu %lu %lu %lu %lu %lu %lu %*d %*d %*d\n",
			&new_total_cpu.utime, &new_total_cpu.ntime, &new_total_cpu.stime,
			&new_total_cpu.itime, &new_total_cpu.iowtime,
			&new_total_cpu.irqtime, &new_total_cpu.sirqtime);

	for (i = 0; i < cpu_count; i++) {
		sprintf(scanline, "cpu%d %%lu %%lu %%lu %%lu %%lu %%lu %%lu %%*d %%*d %%*d\n", i);
		fscanf(file, scanline, &new_cpus[i].utime, &new_cpus[i].ntime,
				&new_cpus[i].stime, &new_cpus[i].itime, &new_cpus[i].iowtime,
				&new_cpus[i].irqtime, &new_cpus[i].sirqtime);
		new_cpus[i].cpu_freqs.freq = get_cpu_freq(i);
	}
	fclose(file);
}
void usage(char* cmd) {
	fprintf(stderr, "Usage %s [-n num]  [ -i interval ] [ -h ]\n"
            "      -n num    loop num times, default is dead loop\n"
			"      -h        show help\n"
			"      -i num    intervals in ms default %d ms\n", cmd, DEFAULT_INTERVAL);
}
//计算total_time
static unsigned long get_cpu_total_time(struct cpu_info *cpu) {
	return (cpu->utime + cpu->ntime + cpu->stime + cpu->itime + cpu->iowtime
			+ cpu->irqtime + cpu->sirqtime);
}

static unsigned long get_cpu_work_time(struct cpu_info *cpu) {
	return (cpu->utime + cpu->ntime + cpu->stime);
}
//计算系统负债100*(total-idle)/total
static int calc_cpu_load(struct cpu_info *new_info, struct cpu_info* old_info)
{
	int total = get_cpu_total_time(new_info) - get_cpu_total_time(old_info);
    if(total == 0)
        return -1;
	//int idle = new_info->itime - old_info->itime;
	//int busy = 100*(total -idle);
    int busy = get_cpu_work_time(new_info) - get_cpu_work_time(old_info);
	return 100*busy/total;
}

//打印系统负载信息
static void print_stats()
{
	int total_load = calc_cpu_load(&new_total_cpu, &old_total_cpu);
	int i = 0;
	printf("cpu:\t%3d\t", total_load);
	for( i=0; i< cpu_count; i++){
		total_load = calc_cpu_load(&(new_cpus[i]), &(old_cpus[i]));
		printf("cpu%d:\t%3d\t%7d\t", i, total_load, new_cpus[i].cpu_freqs.freq);
	}
	printf("\n");

}
int main(int argc, char* argv[]) {
	int interval = DEFAULT_INTERVAL;
	int flags, opt;
	struct cpu_info *tmp_cpus;
    int dead_loop = 1;
    int count = 0;
	while ((opt = getopt(argc, argv, "n:i:h")) != -1) {
		switch (opt) {
		case 'i':
			interval = atoi(optarg);
			break;
        case 'n':
            count = atoi(optarg);
            dead_loop = 0;
            break;
		case 'h':
			usage(argv[0]);
			exit(0);
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
    //获取CPU数量，分配状态数组
	cpu_count = get_cpu_count();
	old_cpus = malloc(sizeof(struct cpu_info) * cpu_count);
	if (!old_cpus)
		die("Could not allocate struct cpu_info");
	new_cpus = malloc(sizeof(struct cpu_info) * cpu_count);
	if (!new_cpus)
		die("Could not allocate struct cpu_info");
	read_stats();
	while (dead_loop || (count-->0)) {
		old_total_cpu = new_total_cpu;
		tmp_cpus = old_cpus;
		old_cpus = new_cpus;
		new_cpus = tmp_cpus;
		usleep(interval * 1000);
		read_stats();
		print_stats();
	}
    free(new_cpus);
    free(old_cpus);
    exit(EXIT_SUCCESS);
}
