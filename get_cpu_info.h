#ifndef GET_CPU_INFO
#define GET_CPU_INFO

typedef struct cpuinfo_element {
	char *name;
	enum _type {
		_int32_t,
		_string,
		_int64_t,
		_uint32_t,
		_error
	} type;
	union _info {
		int32_t _int32_t_info;
		int64_t _int64_t_info;
		uint32_t _uint32_t_info;
		char *_string_info;
	} info;
} cpuinfo_ele;

#define CPUINFOELE(_name,_type) {	\
	.name = _name,					\
	.type = _type					\
}

int get_info();
int show_info();
void free_list();

#endif