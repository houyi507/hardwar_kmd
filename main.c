#include "stdint.h"
#include "opendla.h"
#include "dla_debug.h"
#include "dla_engine.h"
#include "dla_err.h"
#include "dla_interface.h"
#include "dla_engine_internal.h"


#define NULL (0)

struct dla_engine global_engine;

#define DLA_OP_CACHE_SIZE (32) // (DLA_NUM_GROUPS * ((DLA_OP_NUM + 2) * 2))
//#pragma HLS data_pack variable=desc_cache instance=desc_cache_0

#define DMA_BIDIRECTIONAL (65)			//??

static int32_t desc_refcount[DLA_OP_NUM][DLA_OP_CACHE_SIZE];

struct dla_processor global_processors[DLA_OP_NUM];

struct dla_task task_struct;

struct dla_common_op_desc desc_cache[DLA_OP_NUM * DLA_OP_CACHE_SIZE];

#define DLA_LIST_SIZE (64)
#define DLA_LUT_LIST_SIZE (2)

union dla_operation_container operation_desc_list[DLA_LIST_SIZE];
union dla_surface_container surface_desc_list[DLA_LIST_SIZE];
struct dla_lut_param lut_list[DLA_LUT_LIST_SIZE];


int dla_top(struct dla_common_op_desc desc_cache0[DLA_OP_NUM * DLA_OP_CACHE_SIZE], volatile int *nvdla)
{
#pragma HLS INTERFACE m_axi depth=32 port=desc_cache0
#pragma HLS INTERFACE s_axilite port=nvdla bundle=AXI_Lite_1
#pragma HLS INTERFACE s_axilite port=return bundle=AXI_Lite_1

	int ret = 0;

	for (int i = 0; i < DLA_OP_NUM * DLA_OP_CACHE_SIZE; i++)
	{
		desc_cache[i] = desc_cache0[i];
	}


	//memcpy(desc_cache,desc_cache0, 32*sizeof(struct dla_common_op_desc));

	//global_engine.status = nvdla[1];

	struct dla_engine *engine = &global_engine;


	ret = dla_initiate_processors(engine);

	//nvdla[5] = ret;
	return 0;
}

void dla_reg_write(uint32_t *addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave
#pragma HLS INTERFACE m_axi port=reg depth=32 offset = slave

		*(addr) = reg;

}
uint32_t dla_reg_read(const uint32_t *addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave
	uint32_t reg;
	reg = *addr;
	return reg;
}
uint32_t reg_read(const uint32_t *addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

	return dla_reg_read(addr);
}


void reg_write(uint32_t *addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave
#pragma HLS INTERFACE m_axi port=reg depth=32 offset = slave
	dla_reg_write((uint32_t *)addr, reg);
}



void dla_debug(const char *str, ...)
{

}

void dla_info(const char *str, ...)
{

}

void dla_warn(const char *str, ...)
{

}

void dla_error(const char *str, ...)
{

}


#define ROUTE_TO_OP_RET(func_0, id_0, ...) \
int32_t ret = -1; \
	\
switch (id_0) \
{ \
	/*case DLA_OP_BDMA: ret = dla_bdma_##func_0(__VA_ARGS__); break; \
	case DLA_OP_CONV: ret = dla_conv_##func_0(__VA_ARGS__); break; \
	case DLA_OP_SDP: ret = dla_sdp_##func_0(__VA_ARGS__); break; \
	case DLA_OP_PDP: ret = dla_pdp_##func_0(__VA_ARGS__); break; \
	case DLA_OP_CDP: ret = dla_cdp_##func_0(__VA_ARGS__); break; \*/ \
	case DLA_OP_RUBIK: ret = dla_rubik_##func_0(__VA_ARGS__); break; \
	default: break; \
} \
return ret;

#define ROUTE_TO_OP(func_0, id_0, ...) \
	\
switch (id_0) \
{ \
	/*case DLA_OP_BDMA: dla_bdma_##func_0(__VA_ARGS__); break; \
	case DLA_OP_CONV: dla_conv_##func_0(__VA_ARGS__); break; \
	case DLA_OP_SDP: dla_sdp_##func_0(__VA_ARGS__); break; \
	case DLA_OP_PDP: dla_pdp_##func_0(__VA_ARGS__); break; \
	case DLA_OP_CDP: dla_cdp_##func_0(__VA_ARGS__); break; \*/ \
	case DLA_OP_RUBIK: dla_rubik_##func_0(__VA_ARGS__); break; \
	default: break; \
}

int32_t dla_processor_is_ready(processor_id_t id, struct dla_processor *processor,
			  struct dla_processor_group *group)
{
	ROUTE_TO_OP_RET(is_ready, id, processor, group)

}

int32_t dla_processor_enable(processor_id_t id, struct dla_processor_group *group)
{
	ROUTE_TO_OP_RET(enable, id, group)
}

int32_t dla_processor_program(processor_id_t id, struct dla_processor_group *group)
{
	ROUTE_TO_OP_RET(program, id, group)
}

void dla_processor_set_producer(processor_id_t id, int32_t group_id, int32_t rdma_id)
{
	ROUTE_TO_OP(set_producer, id, group_id, rdma_id)
}

void dla_processor_dump_config(processor_id_t id, struct dla_processor_group *group)
{
	ROUTE_TO_OP(dump_config, id, group)
}

void dla_processor_rdma_check(processor_id_t id, struct dla_processor_group *group)
{
	ROUTE_TO_OP(rdma_check, id, group)

}
void dla_processor_get_stat_data(processor_id_t id, struct dla_processor *processor,
			struct dla_processor_group *group)
{
	ROUTE_TO_OP(get_stat_data, id, processor, group)
}

void dla_processor_dump_stat(processor_id_t id, struct dla_processor *processor)
{
	ROUTE_TO_OP(get_stat_data, id, processor)
}




struct dla_common_op_desc
*dla_get_op_desc(struct dla_task *task,
			   int16_t index,
			   uint8_t op_type,
			   uint8_t roi_index)
{
	int32_t i;
	int32_t ret;
	uint64_t op_base;
	uint64_t dep_graph_addr;
	struct dla_common_op_desc *desc = NULL;
	struct dla_engine *engine = dla_get_engine();

	struct dla_common_op_desc desc0;

	struct dla_common_op_desc dst[32];
	uint32_t size;


	if (index == -1) {
		dla_debug("no desc get due to index==-1\n");
		goto exit;
	}

	dep_graph_addr = (sizeof(struct dla_common_op_desc) *
				engine->network.num_operations * roi_index);

	desc = &desc_cache[op_type * 32 + 0];
	desc0 = desc_cache[op_type * 32 + 0];

	for (i = 0; i < DLA_OP_CACHE_SIZE; i++, desc0 = desc_cache[op_type * 32 + i]) {
		if (desc0.index == index && desc0.roi_index == roi_index) {
			if (desc0.op_type != op_type) {
				dla_error("op_cache[op=%u] contains incorrect "
						"entry of op[%u]\n", op_type,
						desc0.op_type);
				continue;
			}
			desc_refcount[op_type][i]++;
			goto exit;
		}
	}

	//desc = &desc_cache[op_type][0];
	desc = &desc_cache[op_type * 32 + 0];
	for (i = 0; i < DLA_OP_CACHE_SIZE; i++, desc++) {
		if (desc->index == -1) {
			/*op_base = dep_graph_addr +
					(sizeof(struct dla_common_op_desc) *
					(uint64_t)index);*/
			/*ret = dla_data_read((void *) desc,
					(void *)task->dependency_graph_addr,
					sizeof(struct dla_common_op_desc)
					);*/
			ret = dla_dep_graph_read((uint16_t)task->dependency_graph_addr,
					(struct dla_common_op_desc) dst[32],
					(uint32_t) size,
					(uint64_t)index);


			if (ret) {
				desc = NULL;
				goto exit;
			}

			if (op_type != desc->op_type) {
				/*
				 * op_type of entry read from DRAM should not
				 * mismatch with given op_type. If they
				 * mismatches, then wrong entry is fetched, so
				 * report this issue by throwing error.
				 */
				dla_error("Fetched [op_type=%u] from DRAM doesn't "
					"match with op_type[%u]\n",
					desc->op_type,
					op_type);
				desc->op_type = op_type;
				desc->index = -1;
				desc->roi_index = -1;
				desc = NULL;
				goto exit;
			}

			desc->index = index;
			desc->roi_index = roi_index;

			/**
			 * Refcount must be 0 if we are reading it first time
			 * from DRAM
			 */
			assert(desc_refcount[op_type][i] == 0);

			desc_refcount[op_type][i]++;
			goto exit;
		}
	}

exit:
	return &desc_cache[op_type * 32 + i];;

}

struct dla_engine *dla_get_engine()
{
	struct dla_engine *engine = &global_engine;

	return engine;
}

// 需要dma_buf API读取数据
// dma_buf_begin_cpu_access(buf, DMA_BIDIRECTIONAL);
// dma_buf_vmap(buf);

int32_t dla_dep_graph_read(uint16_t *src,struct dla_common_op_desc dst[32],uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave


	int i=0;
	struct dla_common_op_desc buf[32];

	memcpy(buf,src,size * sizeof(struct dla_common_op_desc));

	for (i = 0; i < size; i ++)
		{
			dst[i] = buf[i];
		  /*dst[i].index = buf[i].index;
			dst[i].roi_index = buf[i].roi_index;
			dst[i].op_type =  buf[i].op_type;
			dst[i].dependency_count = buf[i].dependency_count;
			dst[i].reserved0[0] = buf[i].reserved0[0];
			dst[i].reserved0[1] = buf[i].reserved0[1];
			dst[i].reserved0[2] = buf[i].reserved0[2];

			for (j = 0; j < 6; j ++)
					{
						dst[i].consumers[j].index = buf[i].consumers[j].index;
						dst[i].consumers[j].event = buf[i].consumers[j].event;
						dst[i].consumers[j].res = buf[i].consumers[j].res;
				   }
			*/
		}
	return 0;

}


int32_t dla_op_config_read(uint16_t *src,union dla_operation_container dst[32],
		uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave

	int i=0;
	union dla_operation_container buf[32];

	memcpy(buf,src,size * sizeof(union dla_operation_container));

	for (i = 0; i < size; i ++)
	{
		dst[i]=buf[i];
	}
	return 0;
}

int32_t dla_surface_config_read(uint8_t *src,union dla_surface_container dst[32],
		uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave

	int i=0;
	union dla_surface_container buf[32];

	memcpy(buf,src,size * sizeof(union dla_surface_container));

	for (i = 0; i < size; i ++)
	{
		dst[i]=buf[i];
	}
return 0;
}

int32_t dla_lut_read(uint16_t *src,struct dla_lut_param dst[32],
		uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave

	int i=0;
	struct dla_lut_param buf[32];

		memcpy(buf,src,size * sizeof(struct dla_lut_param));

	for (i = 0; i < size; i ++)
	{
		dst[i] = buf[i];
	}
	return 0;
}
int32_t dla_dep_graph_write(uint16_t *dst,struct dla_common_op_desc src[32],uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=dst depth=32 offset = slave

	int i=0;
	struct dla_common_op_desc buf[32];

	for (i = 0; i < size; i ++)
		{
			buf[i] = src[i];
		}
	memcpy(dst,buf,size * sizeof(struct dla_common_op_desc));

	return 0;

}

int32_t dla_op_config_write(uint16_t *dst,union dla_operation_container src[32],uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=dst depth=32 offset = slave

	int i=0;
	union dla_operation_container buf[32];

	for (i = 0; i < size; i ++)
		{
			buf[i] = src[i];
		}
	memcpy(dst,buf,size * sizeof(union dla_operation_container));

	return 0;

}

int32_t dla_surface_config_write(uint16_t *dst,union dla_surface_container src[32],uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=dst depth=32 offset = slave

	int i=0;
	union dla_surface_container buf[32];

	for (i = 0; i < size; i ++)
		{
			buf[i] = src[i];
		}
	memcpy(dst,buf,size * sizeof(union dla_surface_container));

	return 0;

}

int32_t dla_lut_write(uint16_t *dst,struct dla_lut_param src[32],uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=dst depth=32 offset = slave

	int i=0;
	struct dla_lut_param buf[32];

	for (i = 0; i < size; i ++)
		{
			buf[i] = src[i];
		}
	memcpy(dst,buf,size * sizeof(struct dla_lut_param));

	return 0;

}
