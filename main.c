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

	global_engine.status = nvdla[1];

	struct dla_engine *engine = &global_engine;


	ret = dla_initiate_processors(engine);

	nvdla[5] = ret;
	return 0;
}

void dla_reg_write(uint32_t *addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

		*(addr) = reg;

}
uint32_t dla_reg_read(uint32_t addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave
	uint32_t reg;
	reg = addr;
	return reg;
}
uint32_t reg_read(uint32_t addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

	return dla_reg_read(addr);
}


void reg_write(uint32_t *addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave
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


	int i=0,j=0;
		for (i = 0; i < size; i ++)
		{
			dst[i].index = *(src + i);
			dst[i].roi_index = *(src + i) + 16;
			dst[i].op_type = *(src + i) + 24;
			dst[i].dependency_count = *(src + i) + 32;
			dst[i].reserved0[0] = *(src + i) + 40;
			dst[i].reserved0[1] = *(src + i) + 48;
			dst[i].reserved0[2] = *(src + i) + 56;

			for (j = 0; j < 6; j ++)
					{
						dst[i].consumers[j].index = *(src + i) + (64 + j * 32 + 0);
						dst[i].consumers[j].event = *(src + i) + (64 + j * 32 + 16);
						dst[i].consumers[j].res = *(src + i) + (64 + j * 32 + 24);

				}
	}
	return 0;

}


int32_t dla_op_config_read(uint16_t *src,union dla_operation_container dst[32],
		uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave
	int i=0,j=0;
	for (i = 0; i < size; i ++)
	{
		//dla_bdma_op_desc
		dst[i].bdma_op.num_transfers = *(src + i);
		dst[i].bdma_op.reserved0 = *(src + i) + 16;
		//dla_conv_op_desc
		dst[i].conv_op.conv_mode = *(src + i) + 32;
		dst[i].conv_op.data_reuse = *(src + i) + 40;
		dst[i].conv_op.weight_reuse = *(src + i) + 48;
		dst[i].conv_op.skip_data_rls = *(src + i) + 56;
		dst[i].conv_op.skip_weight_rls = *(src + i) + 64;
		dst[i].conv_op.reserved0 = *(src + i) + 72;
		dst[i].conv_op.entry_per_slice = *(src + i) + 80;
		dst[i].conv_op. data_format = *(src + i) + 96;
		dst[i].conv_op.pixel_mapping = *(src + i) + 104;
		dst[i].conv_op.fetch_grain = *(src + i) + 112;
		dst[i].conv_op.reserved_b[0] = *(src + i) + 128;
		dst[i].conv_op.reserved_b[1] = *(src + i) + 136;
		dst[i].conv_op.reserved_b[2] = *(src + i) + 144;
		dst[i].conv_op.reserved_b[3] = *(src + i) + 152;
		dst[i].conv_op.reserved_b[4] = *(src + i) + 160;
		dst[i].conv_op.reserved_b[5] = *(src + i) + 168;
		dst[i].conv_op.reserved_b[6] = *(src + i) + 176;
		dst[i].conv_op.reserved_b[7] = *(src + i) + 184;
		dst[i].conv_op.batch = *(src + i) + 192;
		dst[i].conv_op.weight_format = *(src + i) + 200;
		dst[i].conv_op.data_bank = *(src + i) + 208;
		dst[i].conv_op.weight_bank = *(src + i) + 216;
		dst[i].conv_op.batch_stride = *(src + i) + 224;
		dst[i].conv_op.post_extension = *(src + i) + 256;
		dst[i].conv_op.pixel_override = *(src + i) + 264;
		dst[i].conv_op.release = *(src + i) + 272;
		dst[i].conv_op.input_width_csc = *(src + i) + 288;
		dst[i].conv_op.input_height_csc = *(src + i) + 304;
		dst[i].conv_op.input_channel_csc = *(src + i) + 320;
		dst[i].conv_op.kernel_width_csc = *(src + i) + 336;
		dst[i].conv_op.kernel_height_csc = *(src + i) + 352;
		dst[i].conv_op.kernel_channel_csc = *(src + i) + 368;
		dst[i].conv_op.input_width_cmac = *(src + i) + 384;
		dst[i].conv_op.input_height_cmac = *(src + i) + 400;
		dst[i].conv_op.bytes_per_kernel = *(src + i) + 416;
		dst[i].conv_op.mean_ry = *(src + i) + 448;
		dst[i].conv_op.mean_gu = *(src + i) + 464;
		dst[i].conv_op.mean_bv = *(src + i) + 480;
		dst[i].conv_op.mean_ax = *(src + i) + 496;
		dst[i].conv_op.mean_format = *(src + i) + 512;
		dst[i].conv_op.conv_stride_x = *(src + i) + 520;
		dst[i].conv_op.conv_stride_y = *(src + i) + 528;
		dst[i].conv_op.pad_x_left = *(src + i) + 536;
		dst[i].conv_op.pad_x_right = *(src + i) + 544;
		dst[i].conv_op.pad_y_top = *(src + i) + 552;
		dst[i].conv_op.pad_y_bottom = *(src + i) + 560;
		dst[i].conv_op.dilation_x = *(src + i) + 568;
		dst[i].conv_op.dilation_y = *(src + i) + 576;
		dst[i].conv_op.reserved2[0] = *(src + i) + 584;
		dst[i].conv_op.reserved2[1] = *(src + i) + 592;
		dst[i].conv_op.pra_truncate = *(src + i) + 600;
		dst[i].conv_op.in_precision = *(src + i) + 608;
		dst[i].conv_op.out_precision = *(src + i) + 616;
		dst[i].conv_op.pad_val = *(src + i) + 624;
		dst[i].conv_op.in_cvt.scale = *(src + i) + 640;
		dst[i].conv_op.in_cvt.truncate = *(src + i) + 656;
		dst[i].conv_op.in_cvt.enable = *(src + i) + 664;
		dst[i].conv_op.in_cvt.offset = *(src + i) + 672;
		dst[i].conv_op.out_cvt.scale = *(src + i) + 704;
		dst[i].conv_op.out_cvt.truncate = *(src + i) + 720;
		dst[i].conv_op.out_cvt.enable = *(src + i) + 728;
		dst[i].conv_op.out_cvt.offset = *(src + i) + 736;
		//dla_sdp_op_desc
		dst[i].sdp_op.src_precision = *(src + i) + 768;
		dst[i].sdp_op.dst_precision = *(src + i) + 776;
		dst[i].sdp_op.lut_index = *(src + i) + 784;
		dst[i].sdp_op.out_cvt.scale = *(src + i) + 800;
		dst[i].sdp_op.out_cvt.truncate = *(src + i) + 816;
		dst[i].sdp_op.out_cvt.enable = *(src + i) + 824;
		dst[i].sdp_op.out_cvt.offset = *(src + i) + 832;
		dst[i].sdp_op.conv_mode = *(src + i) + 864;
		dst[i].sdp_op.batch_num = *(src + i) + 872;
		dst[i].sdp_op.reserved0 = *(src + i) + 880;
		dst[i].sdp_op.batch_stride = *(src + i) + 896;
		dst[i].sdp_op.x1_op.enable = *(src + i) + 928;
		dst[i].sdp_op.x1_op.alu_type = *(src + i) + 936;
		dst[i].sdp_op.x1_op.type = *(src + i) + 944;
		dst[i].sdp_op.x1_op.mode = *(src + i) + 952;
		dst[i].sdp_op.x1_op.act = *(src + i) + 960;
		dst[i].sdp_op.x1_op.shift_value = *(src + i) + 968;
		dst[i].sdp_op.x1_op.truncate = *(src + i) + 976;
		dst[i].sdp_op.x1_op.precision = *(src + i) + 984;
		dst[i].sdp_op.x1_op.alu_operand = *(src + i) + 992;
		dst[i].sdp_op.x1_op.mul_operand = *(src + i) + 1024;
		dst[i].sdp_op.x1_op.cvt.alu_cvt.scale = *(src + i) + 1056;
		dst[i].sdp_op.x1_op.cvt.alu_cvt.truncate = *(src + i) + 1072;
		dst[i].sdp_op.x1_op.cvt.alu_cvt.enable = *(src + i) + 1080;
		dst[i].sdp_op.x1_op.cvt.alu_cvt.offset = *(src + i) + 1088;
		dst[i].sdp_op.x1_op.cvt.mul_cvt.scale = *(src + i) + 1120;
		dst[i].sdp_op.x1_op.cvt.mul_cvt.truncate = *(src + i) + 1136;
		dst[i].sdp_op.x1_op.cvt.mul_cvt.enable = *(src + i) + 1144;
		dst[i].sdp_op.x1_op.cvt.mul_cvt.offset = *(src + i) + 1152;
		dst[i].sdp_op.x2_op.enable = *(src + i) + 1184;
		dst[i].sdp_op.x2_op.alu_type = *(src + i) + 1192;
		dst[i].sdp_op.x2_op.type = *(src + i) + 1200;
		dst[i].sdp_op.x2_op.mode = *(src + i) + 1208;
		dst[i].sdp_op.x2_op.act = *(src + i) + 1216;
		dst[i].sdp_op.x2_op.shift_value = *(src + i) + 1224;
		dst[i].sdp_op.x2_op.truncate = *(src + i) + 1232;
		dst[i].sdp_op.x2_op.precision = *(src + i) + 1240;
		dst[i].sdp_op.x2_op.alu_operand = *(src + i) + 1248;
		dst[i].sdp_op.x2_op.mul_operand = *(src + i) + 1280;
		dst[i].sdp_op.x2_op.cvt.alu_cvt.scale = *(src + i) + 1312;
		dst[i].sdp_op.x2_op.cvt.alu_cvt.truncate = *(src + i) + 1328;
		dst[i].sdp_op.x2_op.cvt.alu_cvt.enable = *(src + i) + 1336;
		dst[i].sdp_op.x2_op.cvt.alu_cvt.offset = *(src + i) + 1344;
		dst[i].sdp_op.x2_op.cvt.mul_cvt.scale = *(src + i) + 1376;
		dst[i].sdp_op.x2_op.cvt.mul_cvt.truncate = *(src + i) + 1392;
		dst[i].sdp_op.x2_op.cvt.mul_cvt.enable = *(src + i) + 1400;
		dst[i].sdp_op.x2_op.cvt.mul_cvt.offset = *(src + i) + 1408;
		dst[i].sdp_op.y_op.enable = *(src + i) + 1440;
		dst[i].sdp_op.y_op.alu_type = *(src + i) + 1448;
		dst[i].sdp_op.y_op.type = *(src + i) + 1456;
		dst[i].sdp_op.y_op.mode = *(src + i) + 1464;
		dst[i].sdp_op.y_op.act = *(src + i) + 1472;
		dst[i].sdp_op.y_op.shift_value = *(src + i) + 1480;
		dst[i].sdp_op.y_op.truncate = *(src + i) + 1488;
		dst[i].sdp_op.y_op.precision = *(src + i) + 1496;
		dst[i].sdp_op.y_op.alu_operand = *(src + i) + 1504;
		dst[i].sdp_op.y_op.mul_operand = *(src + i) + 1536;
		dst[i].sdp_op.y_op.cvt.alu_cvt.scale = *(src + i) + 1568;
		dst[i].sdp_op.y_op.cvt.alu_cvt.truncate = *(src + i) + 1584;
		dst[i].sdp_op.y_op.cvt.alu_cvt.enable = *(src + i) + 1592;
		dst[i].sdp_op.y_op.cvt.alu_cvt.offset = *(src + i) + 1600;
		dst[i].sdp_op.y_op.cvt.mul_cvt.scale = *(src + i) + 1632;
		dst[i].sdp_op.y_op.cvt.mul_cvt.truncate = *(src + i) + 1648;
		dst[i].sdp_op.y_op.cvt.mul_cvt.enable = *(src + i) + 1656;
		dst[i].sdp_op.y_op.cvt.mul_cvt.offset = *(src + i) + 1664;
		//dla_pdp_op_desc
		dst[i].pdp_op.partial_in_width_first = *(src + i) + 1696;
		dst[i].pdp_op.partial_in_width_mid = *(src + i) + 1712;
		dst[i].pdp_op.partial_in_width_last = *(src + i) + 1728;
		dst[i].pdp_op.partial_width_first = *(src + i) + 1744;
		dst[i].pdp_op.partial_width_mid = *(src + i) + 1760;
		dst[i].pdp_op.partial_width_last = *(src + i) + 1776;
		dst[i].pdp_op.split_num = *(src + i) + 1792;
		dst[i].pdp_op.pool_mode = *(src + i) + 1800;
		dst[i].pdp_op.pool_width = *(src + i) + 1808;
		dst[i].pdp_op.pool_height = *(src + i) + 1816;
		dst[i].pdp_op.stride_x = *(src + i) + 1824;
		dst[i].pdp_op.stride_y = *(src + i) + 1832;
		dst[i].pdp_op.pad_left = *(src + i) + 1840;
		dst[i].pdp_op.pad_right = *(src + i) + 1848;
		dst[i].pdp_op.pad_top = *(src + i) + 1856;
		dst[i].pdp_op.pad_bottom = *(src + i) + 1864;
		dst[i].pdp_op.precision = *(src + i) + 1872;
		dst[i].pdp_op.reserved0 = *(src + i) + 1880;
		dst[i].pdp_op.padding_value[0] = *(src + i) + 1888;
		dst[i].pdp_op.padding_value[1] = *(src + i) + 1920;
		dst[i].pdp_op.padding_value[2] = *(src + i) + 1952;
		dst[i].pdp_op.padding_value[3] = *(src + i) + 1984;
		dst[i].pdp_op.padding_value[4] = *(src + i) + 2016;
		dst[i].pdp_op.padding_value[5] = *(src + i) + 2048;
		dst[i].pdp_op.padding_value[6] = *(src + i) + 2080;
		//dla_cdp_op_desc
		dst[i].cdp_op.in_precision = *(src + i) + 2112;
		dst[i].cdp_op.out_precision = *(src + i) + 2120;
		dst[i].cdp_op.lut_index = *(src + i) + 2128;
		dst[i].cdp_op.in_cvt.scale = *(src + i) + 2144;
		dst[i].cdp_op.in_cvt.truncate = *(src + i) + 2160;
		dst[i].cdp_op.in_cvt.enable = *(src + i) + 2168;
		dst[i].cdp_op.in_cvt.offset = *(src + i) + 2176;
		dst[i].cdp_op.out_cvt.scale = *(src + i) + 2208;
		dst[i].cdp_op.out_cvt.truncate = *(src + i) + 2224;
		dst[i].cdp_op.out_cvt.enable = *(src + i) + 2232;
		dst[i].cdp_op.out_cvt.offset = *(src + i) + 2240;
		dst[i].cdp_op.local_size = *(src + i) + 2272;
		dst[i].cdp_op.bypass_sqsum = *(src + i) + 2280;
		dst[i].cdp_op.bypass_out_mul = *(src + i) + 2288;
		dst[i].cdp_op.reserved0 = *(src + i) + 2296;
		//dla_rubik_op_desc
		dst[i].rubik_op.mode = *(src + i) + 2304;
		dst[i].rubik_op.precision = *(src + i) + 2312;
		dst[i].rubik_op.stride_x = *(src + i) + 2320;
		dst[i].rubik_op.stride_y = *(src + i) + 2328;
	}
	return 0;
}

int32_t dla_surface_config_read(uint8_t *src,union dla_surface_container dst[32],
		uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave
	int i=0,j=0;
	for (i = 0; i < size; i ++)
	{
		//dla_bdma_surface_desc
		dst[i].bdma_surface.source_type = *(src + i);
		dst[i].bdma_surface.destination_type = *(src + i) + 8;
		dst[i].bdma_surface.num_transfers = *(src + i) + 16;
		for (j = 0; j < 20; j ++)
		{
			dst[i].bdma_surface.transfers[j].source_address = *(src + i) + (32 + j * 256 + 0);
			dst[i].bdma_surface.transfers[j].destination_address = *(src + i) + (32 + j * 256 + 16);
			dst[i].bdma_surface.transfers[j].line_size = *(src + i) + (32 + j * 256 + 32);
			dst[i].bdma_surface.transfers[j].line_repeat = *(src + i) + (32 + j * 256 + 64);
			dst[i].bdma_surface.transfers[j].source_line = *(src + i) + (32 + j * 256 + 96);
			dst[i].bdma_surface.transfers[j].destination_line = *(src + i) + (32 + j * 256 + 128);
			dst[i].bdma_surface.transfers[j].surface_repeat = *(src + i) + (32 + j * 256 + 160);
			dst[i].bdma_surface.transfers[j].source_surface = *(src + i) + (32 + j * 256 + 192);
			dst[i].bdma_surface.transfers[j].destination_surface = *(src + i) + (32 + j * 256 + 224);

		}
		//dla_conv_surface_desc
		dst[i].conv_surface.weight_data.type = *(src + i) + 5152;
		dst[i].conv_surface.weight_data.address = *(src + i) + 5158;
		dst[i].conv_surface.weight_data.offset = *(src + i) + 5184;
		dst[i].conv_surface.weight_data.size = *(src + i) + 5216;
		dst[i].conv_surface.weight_data.width = *(src + i) + 5248;
		dst[i].conv_surface.weight_data.height = *(src + i) + 5264;
		dst[i].conv_surface.weight_data.channel = *(src + i) + 5280;
		dst[i].conv_surface.weight_data.reserved0 = *(src + i) + 5260;
		dst[i].conv_surface.weight_data.line_stride = *(src + i) + 5312;
		dst[i].conv_surface.weight_data.surf_stride = *(src + i) + 5344;
		dst[i].conv_surface.weight_data.plane_stride = *(src + i) + 5376;
		dst[i].conv_surface.wmb_data.type= *(src + i) + 5408;
		dst[i].conv_surface.wmb_data.address= *(src + i) + 5424;
		dst[i].conv_surface.wmb_data.offset= *(src + i) + 5440;
		dst[i].conv_surface.wmb_data.size= *(src + i) + 5472;
		dst[i].conv_surface.wmb_data.width= *(src + i) + 5504;
		dst[i].conv_surface.wmb_data.height= *(src + i) + 5520;
		dst[i].conv_surface.wmb_data.channel= *(src + i) + 5536;
		dst[i].conv_surface.wmb_data.reserved0= *(src + i) + 5552;
		dst[i].conv_surface.wmb_data.line_stride= *(src + i) + 5568;
		dst[i].conv_surface.wmb_data.surf_stride= *(src + i) + 5600;
		dst[i].conv_surface.wmb_data.plane_stride= *(src + i) + 5632;
		dst[i].conv_surface.wgs_data.type= *(src + i) + 5664;
		dst[i].conv_surface.wgs_data.address= *(src + i) + 5680;
		dst[i].conv_surface.wgs_data.offset= *(src + i) + 5696;
		dst[i].conv_surface.wgs_data.size= *(src + i) + 5728;
		dst[i].conv_surface.wgs_data.width= *(src + i) + 5760;
		dst[i].conv_surface.wgs_data.height= *(src + i) + 5776;
		dst[i].conv_surface.wgs_data.channel= *(src + i) + 5792;
		dst[i].conv_surface.wgs_data.reserved0= *(src + i) + 5808;
		dst[i].conv_surface.wgs_data.line_stride= *(src + i) + 5824;
		dst[i].conv_surface.wgs_data.surf_stride= *(src + i) + 5856;
		dst[i].conv_surface.wgs_data.plane_stride= *(src + i) + 5888;
		dst[i].conv_surface.src_data.type= *(src + i) + 5920;
		dst[i].conv_surface.src_data.address= *(src + i) + 5936;
		dst[i].conv_surface.src_data.offset= *(src + i) + 5952;
		dst[i].conv_surface.src_data.size= *(src + i) + 5984;
		dst[i].conv_surface.src_data.width= *(src + i) + 6016;
		dst[i].conv_surface.src_data.height= *(src + i) + 6032;
		dst[i].conv_surface.src_data.channel= *(src + i) + 6048;
		dst[i].conv_surface.src_data.reserved0= *(src + i) + 6064;
		dst[i].conv_surface.src_data.line_stride= *(src + i) + 6080;
		dst[i].conv_surface.src_data.surf_stride= *(src + i) + 6112;
		dst[i].conv_surface.src_data.plane_stride= *(src + i) + 6144;
		dst[i].conv_surface.dst_data.type= *(src + i) + 6176;
		dst[i].conv_surface.dst_data.address= *(src + i) + 6192;
		dst[i].conv_surface.dst_data.offset= *(src + i) + 6208;
		dst[i].conv_surface.dst_data.size= *(src + i) + 6240;
		dst[i].conv_surface.dst_data.width= *(src + i) + 6272;
		dst[i].conv_surface.dst_data.height= *(src + i) + 6288;
		dst[i].conv_surface.dst_data.channel= *(src + i) + 6304;
		dst[i].conv_surface.dst_data.reserved0= *(src + i) + 6320;
		dst[i].conv_surface.dst_data.line_stride= *(src + i) + 6336;
		dst[i].conv_surface.dst_data.surf_stride= *(src + i) + 6368;
		dst[i].conv_surface.dst_data.plane_stride= *(src + i) + 6400;
		dst[i].conv_surface.offset_u= *(src + i) + 6432;
		dst[i].conv_surface.in_line_uv_stride= *(src + i) + 6496;
		//dla_sdp_surface_desc
		dst[i].sdp_surface.src_data.type= *(src + i) + 6528;
		dst[i].sdp_surface.src_data.address= *(src + i) + 6544;
		dst[i].sdp_surface.src_data.offset= *(src + i) + 6560;
		dst[i].sdp_surface.src_data.size= *(src + i) + 6592;
		dst[i].sdp_surface.src_data.width= *(src + i) + 6624;
		dst[i].sdp_surface.src_data.height= *(src + i) + 6640;
		dst[i].sdp_surface.src_data.channel= *(src + i) + 6656;
		dst[i].sdp_surface.src_data.reserved0= *(src + i) + 6672;
		dst[i].sdp_surface.src_data.line_stride= *(src + i) + 6688;
		dst[i].sdp_surface.src_data.surf_stride= *(src + i) + 6720;
		dst[i].sdp_surface.src_data.plane_stride= *(src + i) + 6752;
		dst[i].sdp_surface.x1_data.type= *(src + i) + 6784;
		dst[i].sdp_surface.x1_data.address= *(src + i) + 6800;
		dst[i].sdp_surface.x1_data.offset= *(src + i) + 6816;
		dst[i].sdp_surface.x1_data.size= *(src + i) + 6848;
		dst[i].sdp_surface.x1_data.width= *(src + i) + 6880;
		dst[i].sdp_surface.x1_data.height= *(src + i) + 6896;
		dst[i].sdp_surface.x1_data.channel= *(src + i) + 6912;
		dst[i].sdp_surface.x1_data.reserved0= *(src + i) + 6928;
		dst[i].sdp_surface.x1_data.line_stride= *(src + i) + 6944;
		dst[i].sdp_surface.x1_data.surf_stride= *(src + i) + 6976;
		dst[i].sdp_surface.x1_data.plane_stride= *(src + i) + 7008;
		dst[i].sdp_surface.x2_data.type= *(src + i) + 7040;
		dst[i].sdp_surface.x2_data.address= *(src + i) + 7056;
		dst[i].sdp_surface.x2_data.offset= *(src + i) + 7072;
		dst[i].sdp_surface.x2_data.size= *(src + i) + 7104;
		dst[i].sdp_surface.x2_data.width= *(src + i) + 7136;
		dst[i].sdp_surface.x2_data.height= *(src + i) + 7152;
		dst[i].sdp_surface.x2_data.channel= *(src + i) + 7168;
		dst[i].sdp_surface.x2_data.reserved0= *(src + i) + 7184;
		dst[i].sdp_surface.x2_data.line_stride= *(src + i) + 7200;
		dst[i].sdp_surface.x2_data.surf_stride= *(src + i) + 7232;
		dst[i].sdp_surface.x2_data.plane_stride= *(src + i) + 7264;
		dst[i].sdp_surface.y_data.type= *(src + i) + 7296;
		dst[i].sdp_surface.y_data.address= *(src + i) + 7312;
		dst[i].sdp_surface.y_data.offset= *(src + i) + 7328;
		dst[i].sdp_surface.y_data.size= *(src + i) + 7360;
		dst[i].sdp_surface.y_data.width= *(src + i) + 7392;
		dst[i].sdp_surface.y_data.height= *(src + i) + 7408;
		dst[i].sdp_surface.y_data.channel= *(src + i) + 7424;
		dst[i].sdp_surface.y_data.reserved0= *(src + i) + 7440;
		dst[i].sdp_surface.y_data.line_stride= *(src + i) + 7456;
		dst[i].sdp_surface.y_data.surf_stride= *(src + i) + 7488;
		dst[i].sdp_surface.y_data.plane_stride= *(src + i) + 7520;
		dst[i].sdp_surface.dst_data.type= *(src + i) + 7552;
		dst[i].sdp_surface.dst_data.address= *(src + i) + 7568;
		dst[i].sdp_surface.dst_data.offset= *(src + i) + 7584;
		dst[i].sdp_surface.dst_data.size= *(src + i) + 7616;
		dst[i].sdp_surface.dst_data.width= *(src + i) + 7648;
		dst[i].sdp_surface.dst_data.height= *(src + i) + 7664;
		dst[i].sdp_surface.dst_data.channel= *(src + i) + 7680;
		dst[i].sdp_surface.dst_data.reserved0= *(src + i) + 7696;
		dst[i].sdp_surface.dst_data.line_stride= *(src + i) + 7712;
		dst[i].sdp_surface.dst_data.surf_stride= *(src + i) + 7744;
		dst[i].sdp_surface.dst_data.plane_stride= *(src + i) + 7776;
		//dla_pdp_surface_desc
		dst[i].pdp_surface.src_data.type= *(src + i) + 7808;
		dst[i].pdp_surface.src_data.address= *(src + i) + 7824;
		dst[i].pdp_surface.src_data.offset= *(src + i) + 7840;
		dst[i].pdp_surface.src_data.size= *(src + i) + 7872;
		dst[i].pdp_surface.src_data.width= *(src + i) + 7904;
		dst[i].pdp_surface.src_data.height= *(src + i) + 7920;
		dst[i].pdp_surface.src_data.channel= *(src + i) + 7936;
		dst[i].pdp_surface.src_data.reserved0= *(src + i) + 7952;
		dst[i].pdp_surface.src_data.line_stride= *(src + i) + 7968;
		dst[i].pdp_surface.src_data.surf_stride= *(src + i) + 8000;
		dst[i].pdp_surface.src_data.plane_stride= *(src + i) + 8032;
		dst[i].pdp_surface.dst_data.type= *(src + i) + 8064;
		dst[i].pdp_surface.dst_data.address= *(src + i) + 8080;
		dst[i].pdp_surface.dst_data.offset= *(src + i) + 8096;
		dst[i].pdp_surface.dst_data.size= *(src + i) + 8128;
		dst[i].pdp_surface.dst_data.width= *(src + i) + 8160;
		dst[i].pdp_surface.dst_data.height= *(src + i) + 8176;
		dst[i].pdp_surface.dst_data.channel= *(src + i) + 8192;
		dst[i].pdp_surface.dst_data.reserved0= *(src + i) + 8208;
		dst[i].pdp_surface.dst_data.line_stride= *(src + i) + 8224;
		dst[i].pdp_surface.dst_data.surf_stride= *(src + i) + 8256;
		dst[i].pdp_surface.dst_data.plane_stride= *(src + i) + 8288;
		///dla_cdp_surface_desc
		dst[i].cdp_surface.src_data.type= *(src + i) + 8320;
		dst[i].cdp_surface.src_data.address= *(src + i) + 8336;
		dst[i].cdp_surface.src_data.offset= *(src + i) + 8352;
		dst[i].cdp_surface.src_data.size= *(src + i) + 8384;
		dst[i].cdp_surface.src_data.width= *(src + i) + 8416;
		dst[i].cdp_surface.src_data.height= *(src + i) + 8432;
		dst[i].cdp_surface.src_data.channel= *(src + i) + 8448;
		dst[i].cdp_surface.src_data.reserved0= *(src + i) + 8464;
		dst[i].cdp_surface.src_data.line_stride= *(src + i) + 8480;
		dst[i].cdp_surface.src_data.surf_stride= *(src + i) + 8512;
		dst[i].cdp_surface.src_data.plane_stride= *(src + i) + 8544;
		dst[i].cdp_surface.dst_data.type= *(src + i) + 8576;
		dst[i].cdp_surface.dst_data.address= *(src + i) + 8592;
		dst[i].cdp_surface.dst_data.offset= *(src + i) + 8608;
		dst[i].cdp_surface.dst_data.size= *(src + i) + 8640;
		dst[i].cdp_surface.dst_data.width= *(src + i) + 8672;
		dst[i].cdp_surface.dst_data.height= *(src + i) + 8688;
		dst[i].cdp_surface.dst_data.channel= *(src + i) + 8704;
		dst[i].cdp_surface.dst_data.reserved0= *(src + i) + 8720;
		dst[i].cdp_surface.dst_data.line_stride= *(src + i) + 8736;
		dst[i].cdp_surface.dst_data.surf_stride= *(src + i) + 8768;
		dst[i].cdp_surface.dst_data.plane_stride= *(src + i) + 8800;
	    ///dla_rubik_surface_desc
		dst[i].rubik_surface.src_data.type= *(src + i) + 8832;
		dst[i].rubik_surface.src_data.address= *(src + i) + 8848;
		dst[i].rubik_surface.src_data.offset= *(src + i) + 8864;
		dst[i].rubik_surface.src_data.size= *(src + i) + 8896;
		dst[i].rubik_surface.src_data.width= *(src + i) + 8928;
		dst[i].rubik_surface.src_data.height= *(src + i) + 8944;
		dst[i].rubik_surface.src_data.channel= *(src + i) + 8960;
		dst[i].rubik_surface.src_data.reserved0= *(src + i) + 8976;
		dst[i].rubik_surface.src_data.line_stride= *(src + i) + 8992;
		dst[i].rubik_surface.src_data.surf_stride= *(src + i) + 9024;
		dst[i].rubik_surface.src_data.plane_stride= *(src + i) + 9056;
		dst[i].rubik_surface.dst_data.type= *(src + i) + 9088;
		dst[i].rubik_surface.dst_data.address= *(src + i) + 9104;
		dst[i].rubik_surface.dst_data.offset= *(src + i) + 9120;
		dst[i].rubik_surface.dst_data.size= *(src + i) + 9152;
		dst[i].rubik_surface.dst_data.width= *(src + i) + 9184;
		dst[i].rubik_surface.dst_data.height= *(src + i) + 9200;
		dst[i].rubik_surface.dst_data.channel= *(src + i) + 9216;
		dst[i].rubik_surface.dst_data.reserved0= *(src + i) + 9232;
		dst[i].rubik_surface.dst_data.line_stride= *(src + i) + 9248;
		dst[i].rubik_surface.dst_data.surf_stride= *(src + i) + 9280;
		dst[i].rubik_surface.dst_data.plane_stride= *(src + i) + 9312;
}
return 0;
}

int32_t dla_lut_read(uint16_t *src,struct dla_lut_param dst[32],
		uint32_t size,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=32 offset = slave
	int i=0,j=0;
	for (i = 0; i < size; i ++)
	{
		for (j = 0; j < 65; j ++)
		{
			dst[i].linear_exp_table[j] = *(src + i) +  (j * 16);
		}
		for (j = 0; j < 257; j ++)
		{
			dst[i].linear_only_table[j] = *(src + i) +  (1040 + j * 16);
		}
		dst[i].linear_exp_offset.exp_offset = *(src + i) + 5152;
		dst[i].linear_exp_offset.frac_bits = *(src + i) + 5160;
		dst[i].linear_exp_offset.reserved0 = *(src + i) + 5168;
		dst[i].linear_only_offset.exp_offset= *(src + i) + 5184;
		dst[i].linear_only_offset.frac_bits = *(src + i) + 5192;
		dst[i].linear_only_offset.reserved0 = *(src + i) + 5200;
		dst[i].linear_exp_start = *(src + i) + 5216;
		dst[i].linear_exp_end = *(src + i) + 5280;
		dst[i].linear_only_start = *(src + i) + 5344;
		dst[i].linear_only_end = *(src + i) + 5408;
		dst[i].linear_exp_underflow_slope.data_i.scale = *(src + i) + 5472;
		dst[i].linear_exp_underflow_slope.data_i.shifter = *(src + i) + 5488;
		dst[i].linear_exp_underflow_slope.data_i.reserved0 = *(src + i) + 5496;
		dst[i].linear_exp_underflow_slope.data_f = *(src + i) + 5504;
	    dst[i].linear_exp_overflow_slope.data_i.scale = *(src + i) + 5520;
	    dst[i].linear_exp_overflow_slope.data_i.shifter = *(src + i) + 5536;
		dst[i].linear_exp_overflow_slope.data_i.reserved0 = *(src + i) + 5544;
		dst[i].linear_exp_overflow_slope.data_f = *(src + i) + 5552;
		dst[i].linear_only_underflow_slope.data_i.scale = *(src + i) + 5568;
		dst[i].linear_only_underflow_slope.data_i.shifter = *(src + i) + 5584;
		dst[i].linear_only_underflow_slope.data_i.reserved0 = *(src + i) + 5592;
		dst[i].linear_only_underflow_slope.data_f = *(src + i) + 5600;
		dst[i].linear_only_overflow_slope.data_i.scale = *(src + i) + 5616;
		dst[i].linear_only_overflow_slope.data_i.shifter = *(src + i) + 5632;
		dst[i].linear_only_overflow_slope.data_i.reserved0 = *(src + i) + 5640;
		dst[i].linear_only_overflow_slope.data_f = *(src + i) + 5648;
		dst[i].hybrid_priority = *(src + i) + 5664;
		dst[i].underflow_priority = *(src + i) + 5672;
		dst[i].overflow_priority = *(src + i) + 5680;
		dst[i].method = *(src + i) + 5688;
	}
	return 0;
}


