#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <string.h>
#include <sys/errno.h>

#include "get_cpu_info.h"

cpuinfo_ele list[] = {
	CPUINFOELE("machdep.cpu.address_bits.physical", _int32_t),
	CPUINFOELE("machdep.cpu.address_bits.virtual", _int32_t),
	CPUINFOELE("machdep.cpu.brand", _int32_t),
	CPUINFOELE("machdep.cpu.brand_string", _string),
	CPUINFOELE("machdep.cpu.cache.L2_associativity", _int32_t),
	CPUINFOELE("machdep.cpu.cache.linesize", _int32_t),
	CPUINFOELE("machdep.cpu.cache.size", _int32_t),
	CPUINFOELE("machdep.cpu.core_count", _int32_t),
	CPUINFOELE("machdep.cpu.cores_per_package", _int32_t),
	CPUINFOELE("machdep.cpu.extfamily", _int32_t),
	CPUINFOELE("machdep.cpu.extfeature_bits", _int64_t),
	CPUINFOELE("machdep.cpu.extfeatures", _string),
	CPUINFOELE("machdep.cpu.extmodel", _int32_t),
	CPUINFOELE("machdep.cpu.family", _int32_t),
	CPUINFOELE("machdep.cpu.feature_bits", _int64_t),
	CPUINFOELE("machdep.cpu.features", _string),
	CPUINFOELE("machdep.cpu.leaf7_feature_bits", _uint32_t),
	CPUINFOELE("machdep.cpu.leaf7_features", _string),
	CPUINFOELE("machdep.cpu.logical_per_package", _int32_t),
	CPUINFOELE("machdep.cpu.max_basic", _uint32_t),
	CPUINFOELE("machdep.cpu.max_ext", _uint32_t),
	CPUINFOELE("machdep.cpu.microcode_version", _int32_t),
	CPUINFOELE("machdep.cpu.model", _int32_t),
	CPUINFOELE("machdep.cpu.processor_flag", _int32_t),
	CPUINFOELE("machdep.cpu.signature", _int32_t),
	CPUINFOELE("machdep.cpu.stepping", _int32_t),
	CPUINFOELE("machdep.cpu.thread_count", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.data.large", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.data.large_level1", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.data.small", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.data.small_level1", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.inst.large", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.inst.small", _int32_t),
	CPUINFOELE("machdep.cpu.tlb.shared", _int32_t),
	CPUINFOELE("machdep.cpu.ucupdate", _int32_t),
	CPUINFOELE("machdep.cpu.vendor", _string),
	CPUINFOELE("machdep.cpu.xsave.extended_state", _uint32_t),
	CPUINFOELE("machdep.tsc.deep_idle_rebase", _uint32_t),
	CPUINFOELE("machdep.tsc.frequency", _int64_t),
	CPUINFOELE("machdep.tsc.nanotime.generation", _uint32_t),
	CPUINFOELE("machdep.tsc.nanotime.shift", _uint32_t)
};

int main() {
	int ret = 0;
	ret = get_info();
	ret = show_info();
	free_list();
	return ret;
}

int get_info() {
	int ret = 0;
	void *info = NULL;
	size_t len = 0;
	size_t list_size = sizeof(list) / sizeof(cpuinfo_ele);
	int i;
	for (i = 0; i < list_size; i++) {
		ret = sysctlbyname(list[i].name, NULL, &len, NULL, 0);
		if (ret) {
			list[i].type = _error;
			continue;
		}
		info = malloc(len);
		if (!info) {
			ret = -ENOMEM;
			goto out;
		}
		ret = sysctlbyname(list[i].name, info, &len, NULL, 0);
		if (ret) {
			goto out;
		}
		switch (list[i].type) {
			case _int32_t:
				list[i].info._int32_t_info = *((int32_t*)info);
				break;
			case _string:
				list[i].info._string_info = strdup((char *)info);
				break;
			case _int64_t:
				list[i].info._int64_t_info = *((int64_t*)info);
				break;
			case _uint32_t:
				list[i].info._uint32_t_info = *((uint32_t*)info);
				break;
			case _error:
				/* Do nothing */
				break;
			default:
				ret = -EINVAL;
				goto out;
		}
		free(info);
		info = NULL;
	}
out:
	if (info) {
		free(info);
		info = NULL;
	}
	return ret;
}

int show_info() {
	int ret = 0;
	size_t list_size = sizeof(list) / sizeof(cpuinfo_ele);
	int i;
	for (i = 0; i < list_size; i++) {
		switch (list[i].type) {
			case _int32_t:
				printf("%s: %d\n", list[i].name, list[i].info._int32_t_info);
				break;
			case _string:
				printf("%s: %s\n", list[i].name, list[i].info._string_info);
				break;
			case _int64_t:
				printf("%s: %lld\n", list[i].name, list[i].info._int64_t_info);
				break;
			case _uint32_t:
				printf("%s: %u\n", list[i].name, list[i].info._uint32_t_info);
				break;
			case _error:
				printf("%s: ERROR\n", list[i].name);
				break;
			default:
				ret = -EINVAL;
				goto out;
		}
	}
out:
	return ret;
}

void free_list() {
	size_t list_size = sizeof(list) / sizeof(cpuinfo_ele);
	int i;
	for (i = 0; i < list_size; i++) {
		if (list[i].type == _string) {
			free(list[i].info._string_info);
			list[i].info._string_info = NULL;
		}
	}
}