#include "stdint.h"
#include "ap_cint.h"
#include "string.h"
#include "opendla_initial.h"
#include "dla_engine_internal.h"
//#include "opendla.h"
//#include "dla_debug.h"
//#include "dla_err.h"
//#include "dla_interface.h"

//#include "common.h"
//#include "engine_debug.h"

//typedef uint1 data_bool;
//typedef uint288 data_type;
#include "types.h"

typedef volatile uint64_t *axi_bus_t;

#define PRECISION_INT8		0
#define PRECISION_INT16		1
#define PRECISION_FP16		2
#define DLA_MEM_HW			2




int32_t op_desc_to_pdp(struct dla_operation_container operation, struct dla_pdp_op_desc *op)
{
		op->partial_in_width_first = operation.data[0];
		op->partial_in_width_mid  = operation.data[0] >> 16;

		op->partial_in_width_last = operation.data[1];
		op-> partial_width_first = operation.data[1] >> 16;

		op->partial_width_mid = operation.data[2];
		op->partial_width_last = operation.data[2] >> 16;

		op->split_num = operation.data[3];
		op->pool_mode = operation.data[3] >> 8;
		op->pool_width = operation.data[3] >> 16;
		op->pool_height = operation.data[3] >> 24;

		op->stride_x = operation.data[4];
		op->stride_y = operation.data[4] >> 8;
		op->pad_left = operation.data[4] >> 16;
		op->pad_right = operation.data[4] >> 24;

		op->pad_top = operation.data[5];
		op->pad_bottom = operation.data[5] >> 8;
		op->precision = operation.data[5] >> 16;
		op->reserved0 = operation.data[5] >> 24;

		op->padding_value[0] = operation.data[6];
		op->padding_value[1] = operation.data[7];
		op->padding_value[2] = operation.data[8];
		op->padding_value[3] = operation.data[9];
		op->padding_value[4] = operation.data[10];
		op->padding_value[5] = operation.data[11];
		op->padding_value[6] = operation.data[12];

	return 0;
}
int32_t pdp_desc_to_op(struct dla_operation_container *operation, struct dla_pdp_op_desc op)
{
	        operation->data[0] = ((uint32_t)op.partial_in_width_first) |
		    		             ((uint32_t)op.partial_in_width_mid  << 16);

		    operation->data[1] = ((uint32_t)op.partial_in_width_last) |
		    					 ((uint32_t)op.partial_width_first << 16);

		    operation->data[2] = ((uint32_t)op.partial_width_mid) |
		    		             ((uint32_t)op.partial_width_last << 16);

		    operation->data[3] = ((uint32_t)op.split_num) |
		    					 ((uint32_t)op.pool_mode << 8) |
		    					 ((uint32_t)op.pool_width << 16) |
		    					 ((uint32_t)op.pool_height << 24);

		    operation->data[4] = ((uint32_t)op.stride_x) |
		    					 ((uint32_t)op.stride_y << 8) |
		    					 ((uint32_t)op.pad_left << 16) |
		    					 ((uint32_t)op.pad_right << 24);

		    operation->data[5] = ((uint32_t)op.pad_top) |
		    					 ((uint32_t)op.pad_bottom << 8) |
		    					 ((uint32_t)op.precision << 16) |
		    					 ((uint32_t)op.reserved0 << 24);

		    operation->data[6] =  op.padding_value[0];
		    operation->data[7] =  op.padding_value[1];
		    operation->data[8] =  op.padding_value[2];
		    operation->data[9] =  op.padding_value[3];
		    operation->data[10] = op.padding_value[4];
		    operation->data[11] = op.padding_value[5];
		    operation->data[12] = op.padding_value[6];

	return 0;
}
int32_t dla_op_config_read(uint32_t *src,struct dla_operation_container *dst,uint64_t index)
{
#pragma HLS INTERFACE m_axi port=src depth=64 offset = slave

	int i;
	uint32_t buf[29];

	memcpy(buf,src + (index * sizeof(struct dla_operation_container)) / 4,sizeof(struct dla_operation_container));

	for(i = 0; i < 29; i++)
	{
		*(dst->data + i) = *(buf + i);
	}

	return 0;
}
int32_t dla_op_config_write(uint32_t *dst,struct dla_operation_container *src,uint64_t index)
{
#pragma HLS INTERFACE m_axi depth=64 port=dst offset = slave

	int i;
	uint32_t buf[29];

	for(i = 0; i < 29; i++)
		{
			*(buf + i) = *(src->data + i);
		}

	memcpy(dst + (index * sizeof(struct dla_operation_container)) / 8,buf,sizeof(struct dla_operation_container));

	return 0;
}
void dla_reg_write(uint32_t *addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

		*(addr) = reg;


}
void reg_write(uint32_t addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

	dla_reg_write((uint32_t *)addr, reg);
}
uint32_t dla_reg_read(const uint32_t *addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave
	uint32_t reg;

	reg = *(addr);
	return reg;
}
uint32_t reg_read(const uint32_t addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

	return dla_reg_read((uint32_t *)addr);
}


void dla_pdp_set_producer(int32_t group_id, int32_t rdma_group_id)
{
	uint32_t reg;

	printf("Enter: %s", __func__);

	printf("group id %d rdma id %d\n", group_id, rdma_group_id);

	reg = group_id << SHIFT(PDP_S_POINTER_0, PRODUCER);
	pdp_reg_write(S_POINTER, reg);

	reg = rdma_group_id << SHIFT(PDP_RDMA_S_POINTER_0, PRODUCER);
	pdp_rdma_reg_write(S_POINTER, reg);

	printf("Exit: %s\n", __func__);
}
int32_t dla_enable_intr(uint32_t mask)
{
	uint32_t reg = glb_reg_read(S_INTR_MASK);

	reg = reg & (~mask);
	glb_reg_write(S_INTR_MASK, reg);

	RETURN(0);
}
static uint32_t
get_fly_mode(uint8_t type)
{
	uint32_t val;

	val = type == DLA_MEM_HW ?
			FIELD_ENUM(PDP_D_OPERATION_MODE_CFG_0,
						FLYING_MODE, ON_FLYING) :
			FIELD_ENUM(PDP_D_OPERATION_MODE_CFG_0,
						FLYING_MODE, OFF_FLYING);

	return val;
}


int write_pdp_regs(struct dla_pdp_op_desc pdp_op)
{
	uint32_t reg, high, low;
	uint64_t input_address = 0;
	uint64_t output_address = 0;


	pdp_rdma_reg_write(D_DATA_CUBE_IN_WIDTH, 1);
	pdp_rdma_reg_write(D_DATA_CUBE_IN_HEIGHT, 1);
	pdp_rdma_reg_write(D_DATA_CUBE_IN_CHANNEL, 1);

	high = HIGH32BITS(input_address);
	low  = LOW32BITS(input_address);

	pdp_rdma_reg_write(D_SRC_BASE_ADDR_HIGH, high);
	pdp_rdma_reg_write(D_SRC_BASE_ADDR_LOW, low);
	pdp_rdma_reg_write(D_SRC_LINE_STRIDE, 1);
	pdp_rdma_reg_write(D_SRC_SURFACE_STRIDE, 1);

	reg = (map_precision[pdp_op.precision] << SHIFT(PDP_RDMA_D_DATA_FORMAT_0, INPUT_DATA));
	pdp_rdma_reg_write(D_DATA_FORMAT, reg);

	reg = map_ram[2] << SHIFT(PDP_RDMA_D_SRC_RAM_CFG_0, SRC_RAM_TYPE);
	pdp_rdma_reg_write(D_SRC_RAM_CFG, reg);

	reg = ((pdp_op.split_num - 1) << SHIFT(PDP_RDMA_D_OPERATION_MODE_CFG_0, SPLIT_NUM));
	pdp_rdma_reg_write(D_OPERATION_MODE_CFG, reg);

	reg = (map_pool_kernel[pdp_op.pool_width] << SHIFT(PDP_RDMA_D_POOLING_KERNEL_CFG_0, KERNEL_WIDTH)) |
		  ((pdp_op.stride_x - 1) << SHIFT(PDP_RDMA_D_POOLING_KERNEL_CFG_0, KERNEL_STRIDE_WIDTH));
	pdp_rdma_reg_write(D_POOLING_KERNEL_CFG, reg);

	reg = (pdp_op.pad_left << SHIFT(PDP_RDMA_D_POOLING_PADDING_CFG_0, PAD_WIDTH));
	pdp_rdma_reg_write(D_POOLING_PADDING_CFG, reg);

	reg = ((pdp_op.partial_in_width_first == 0 ? 0 :
			pdp_op.partial_in_width_first - 1)
		<< SHIFT(PDP_RDMA_D_PARTIAL_WIDTH_IN_0,
			PARTIAL_WIDTH_IN_FIRST)) |
		  ((pdp_op.partial_in_width_mid == 0 ? 0 :
			pdp_op.partial_in_width_mid - 1)
		<< SHIFT(PDP_RDMA_D_PARTIAL_WIDTH_IN_0,
			PARTIAL_WIDTH_IN_MID)) |
		  ((pdp_op.partial_in_width_last == 0 ? 0 :
			pdp_op.partial_in_width_last - 1)
		<< SHIFT(PDP_RDMA_D_PARTIAL_WIDTH_IN_0,
			PARTIAL_WIDTH_IN_LAST));
	pdp_rdma_reg_write(D_PARTIAL_WIDTH_IN, reg);

	reg = (1 << SHIFT(PDP_D_DATA_CUBE_IN_WIDTH_0, CUBE_IN_WIDTH));
	pdp_reg_write(D_DATA_CUBE_IN_WIDTH, reg);

	reg = (1 << SHIFT(PDP_D_DATA_CUBE_IN_HEIGHT_0, CUBE_IN_HEIGHT));
	pdp_reg_write(D_DATA_CUBE_IN_HEIGHT, reg);

	reg = (1 << SHIFT(PDP_D_DATA_CUBE_IN_CHANNEL_0, CUBE_IN_CHANNEL));
	pdp_reg_write(D_DATA_CUBE_IN_CHANNEL, reg);

	reg = (1 << SHIFT(PDP_D_DATA_CUBE_OUT_WIDTH_0, CUBE_OUT_WIDTH));
	pdp_reg_write(D_DATA_CUBE_OUT_WIDTH, reg);

	reg = (1 << SHIFT(PDP_D_DATA_CUBE_OUT_HEIGHT_0, CUBE_OUT_HEIGHT));
	pdp_reg_write(D_DATA_CUBE_OUT_HEIGHT, reg);

	reg = (1 << SHIFT(PDP_D_DATA_CUBE_OUT_CHANNEL_0, CUBE_OUT_CHANNEL));
	pdp_reg_write(D_DATA_CUBE_OUT_CHANNEL, reg);

	reg = (map_pool[pdp_op.pool_mode]
	    << SHIFT(PDP_D_OPERATION_MODE_CFG_0, POOLING_METHOD)) |
		  (get_fly_mode(2)
		<< SHIFT(PDP_D_OPERATION_MODE_CFG_0, FLYING_MODE)) |
		  ((pdp_op.split_num - 1)
		<< SHIFT(PDP_D_OPERATION_MODE_CFG_0, SPLIT_NUM));
	pdp_reg_write(D_OPERATION_MODE_CFG, reg);

	reg = ((pdp_op.partial_in_width_first == 0 ? 0 :
			pdp_op.partial_in_width_first-1)
		<< SHIFT(PDP_D_PARTIAL_WIDTH_IN_0, PARTIAL_WIDTH_IN_FIRST)) |
		  ((pdp_op.partial_in_width_mid == 0 ? 0 :
			pdp_op.partial_in_width_mid-1)
		<< SHIFT(PDP_D_PARTIAL_WIDTH_IN_0, PARTIAL_WIDTH_IN_MID)) |
		  ((pdp_op.partial_in_width_last == 0 ? 0 :
			pdp_op.partial_in_width_last-1)
		<< SHIFT(PDP_D_PARTIAL_WIDTH_IN_0, PARTIAL_WIDTH_IN_LAST));
	pdp_reg_write(D_PARTIAL_WIDTH_IN, reg);

	reg = ((pdp_op.partial_width_first == 0 ? 0 :
			pdp_op.partial_width_first-1)
		<< SHIFT(PDP_D_PARTIAL_WIDTH_OUT_0, PARTIAL_WIDTH_OUT_FIRST)) |
		  ((pdp_op.partial_width_mid == 0 ? 0 :
			pdp_op.partial_width_mid-1)
		<< SHIFT(PDP_D_PARTIAL_WIDTH_OUT_0, PARTIAL_WIDTH_OUT_MID))   |
		  ((pdp_op.partial_width_last == 0 ? 0 :
			pdp_op.partial_width_last-1)
		<< SHIFT(PDP_D_PARTIAL_WIDTH_OUT_0, PARTIAL_WIDTH_OUT_LAST));
	pdp_reg_write(D_PARTIAL_WIDTH_OUT, reg);

	reg = (map_pool_kernel[pdp_op.pool_width]
	   << SHIFT(PDP_D_POOLING_KERNEL_CFG_0, KERNEL_WIDTH)) |
		  (map_pool_kernel[pdp_op.pool_height]
	   << SHIFT(PDP_D_POOLING_KERNEL_CFG_0, KERNEL_HEIGHT))|
		 ((pdp_op.stride_x - 1)
	   << SHIFT(PDP_D_POOLING_KERNEL_CFG_0, KERNEL_STRIDE_WIDTH)) |
		((pdp_op.stride_y - 1)
	   << SHIFT(PDP_D_POOLING_KERNEL_CFG_0, KERNEL_STRIDE_HEIGHT));
	pdp_reg_write(D_POOLING_KERNEL_CFG, reg);

	pdp_reg_write(D_RECIP_KERNEL_WIDTH, recip_kernel_size[pdp_op.precision == PRECISION_FP16][pdp_op.pool_width]);
	pdp_reg_write(D_RECIP_KERNEL_HEIGHT, recip_kernel_size[pdp_op.precision == PRECISION_FP16][pdp_op.pool_height]);

	reg = (pdp_op.pad_left
			<< SHIFT(PDP_D_POOLING_PADDING_CFG_0, PAD_LEFT)) |
			(pdp_op.pad_right
			<< SHIFT(PDP_D_POOLING_PADDING_CFG_0, PAD_RIGHT)) |
			(pdp_op.pad_top
			<< SHIFT(PDP_D_POOLING_PADDING_CFG_0, PAD_TOP)) |
			(pdp_op.pad_bottom
			<< SHIFT(PDP_D_POOLING_PADDING_CFG_0, PAD_BOTTOM));
//		if (pdp_op->precision == PRECISION_FP16) {
//			int32_t i;
//
//			for (i = 0; i < 7; i++)
//				ASSERT_GOTO(pdp_op->padding_value[i] == 0, ret,
//							ERR(INVALID_INPUT), exit);
//		}

		pdp_reg_write(D_POOLING_PADDING_CFG, reg);
		pdp_reg_write(D_POOLING_PADDING_VALUE_1_CFG, pdp_op.padding_value[0]);
		pdp_reg_write(D_POOLING_PADDING_VALUE_2_CFG, pdp_op.padding_value[1]);
		pdp_reg_write(D_POOLING_PADDING_VALUE_3_CFG, pdp_op.padding_value[2]);
		pdp_reg_write(D_POOLING_PADDING_VALUE_4_CFG, pdp_op.padding_value[3]);
		pdp_reg_write(D_POOLING_PADDING_VALUE_5_CFG, pdp_op.padding_value[4]);
		pdp_reg_write(D_POOLING_PADDING_VALUE_6_CFG, pdp_op.padding_value[5]);
		pdp_reg_write(D_POOLING_PADDING_VALUE_7_CFG, pdp_op.padding_value[6]);

//		if (pdp_surface->src_data.type != DLA_MEM_HW) {
//			pdp_reg_write(D_SRC_LINE_STRIDE,
//					pdp_surface->src_data.line_stride);
//			pdp_reg_write(D_SRC_SURFACE_STRIDE,
//					pdp_surface->src_data.surf_stride);
//		}

		high = HIGH32BITS(output_address);
		low = LOW32BITS(output_address);
		pdp_reg_write(D_DST_BASE_ADDR_LOW, low);
		pdp_reg_write(D_DST_BASE_ADDR_HIGH, high);

		pdp_reg_write(D_DST_LINE_STRIDE, 2);
		pdp_reg_write(D_DST_SURFACE_STRIDE, 2);

		reg = (map_ram[2] << SHIFT(PDP_D_DST_RAM_CFG_0, DST_RAM_TYPE));
		pdp_reg_write(D_DST_RAM_CFG, reg);

		reg = (map_precision[pdp_op.precision] << SHIFT(PDP_D_DATA_FORMAT_0, INPUT_DATA));
		pdp_reg_write(D_DATA_FORMAT, reg);

	return 0;
}


int dla_top(struct dla_config config,
		axi_bus_t op_desc_addr)
{
#pragma HLS INTERFACE m_axi depth=64 port=op_desc_addr
//#pragma HLS INTERFACE m_axi depth=64 port=nvdla_base
//#pragma HLS INTERFACE s_axilite port=return
//#pragma HLS INTERFACE s_axilite port=config

		struct dla_operation_container op_config;
		struct dla_pdp_op_desc pdp_op;

		struct dla_processor_group *group;

		group->id = 0;
		group->rdma_id = 0;
		group->pending = 1;
		group->programming = 1;

		//read op_config
		dla_op_config_read(op_desc_addr, &op_config, 0);
		group->operation_desc = &op_config;

		// pdp_op
		op_desc_to_pdp(op_config, &pdp_op);
		group->pdp_op = pdp_op;

		dla_pdp_set_producer(group->id, group->rdma_id);  //write pdp pointer reg

		//dla_enable_intr(MASK(GLB_S_INTR_MASK_0, PDP_DONE_MASK1) | MASK(GLB_S_INTR_MASK_0, PDP_DONE_MASK0));

      write_pdp_regs(pdp_op);
		return 0;
}

