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



int32_t op_desc_to_bdma(struct dla_operation_container operation, struct dla_bdma_op_desc *op)
{
	op->num_transfers = operation.data[0];
	op->reserved0 = operation.data[0] >> 16;
	return 0;
}
int32_t bdma_desc_to_op(struct dla_operation_container *operation, struct dla_bdma_op_desc op)
{
	operation->data[0] = ((uint32_t)op.num_transfers) |
						 ((uint32_t)op.reserved0 << 16);
	return 0;
}
int32_t op_desc_to_conv(struct dla_operation_container operation, struct dla_conv_op_desc *op)
{
		op->conv_mode = operation.data[0];
		op->data_reuse = operation.data[0] >> 8;
		op->weight_reuse = operation.data[0] >> 16;
		op->skip_data_rls = operation.data[0] >> 24;
		op->skip_weight_rls = operation.data[1];
		op->reserved0 = operation.data[1] >> 8;
		op->entry_per_slice = operation.data[1] >> 16;
		op->data_format = operation.data[2];
		op->pixel_mapping = operation.data[2] >> 8;
		op->fetch_grain = operation.data[2] >> 16;
		op->reserved_b[0] = operation.data[3];
		op->reserved_b[1] = operation.data[3] >> 8;
		op->reserved_b[2] = operation.data[3] >> 16;
		op->reserved_b[3] = operation.data[3] >> 24;
		op->reserved_b[4] = operation.data[4];
		op->reserved_b[5] = operation.data[4] >> 8;
		op->reserved_b[6] = operation.data[4] >> 16;
		op->reserved_b[7] = operation.data[4] >> 24;
		op->batch = operation.data[5];
		op->weight_format = operation.data[5] >> 8;
		op->data_bank = operation.data[5] >> 16;
		op->weight_bank = operation.data[5] >> 24;
		op->batch_stride = operation.data[6];
		op->post_extension = operation.data[7];
		op->pixel_override = operation.data[7] >> 8;
		op->release = operation.data[7] >> 16;
		op->input_width_csc = operation.data[8];
		op->input_height_csc = operation.data[8] >> 16;
		op->input_channel_csc = operation.data[9];
		op->kernel_width_csc = operation.data[9] >> 16;
		op->kernel_height_csc = operation.data[10];
		op->kernel_channel_csc = operation.data[10] >> 16;
		op->input_width_cmac = operation.data[11];
	    op->input_height_cmac = operation.data[11] >> 16;
		op->bytes_per_kernel = operation.data[12];
		op->mean_ry = operation.data[13];
		op->mean_gu = operation.data[13] >> 16;
		op->mean_bv = operation.data[14];
		op->mean_ax = operation.data[14] >> 16;
		op->mean_format = operation.data[15];
		op->conv_stride_x = operation.data[15] >> 8;
		op->conv_stride_y = operation.data[15] >> 16;
		op->pad_x_left = operation.data[15] >> 24;
		op->pad_x_right = operation.data[16];
		op->pad_y_top = operation.data[16] >> 8;
		op->pad_y_bottom = operation.data[16] >> 16;
		op->dilation_x = operation.data[16] >> 24;
		op->dilation_y = operation.data[17];
		op->reserved2[0] = operation.data[17] >> 8;
		op->reserved2[1] = operation.data[17] >> 16;
		op->pra_truncate = operation.data[17] >> 24;
		op->in_precision = operation.data[18];
		op->out_precision = operation.data[18] >> 8;
		op->pad_val = operation.data[18] >> 16;
		op->in_cvt.scale = operation.data[19];
		op->in_cvt.truncate = operation.data[19] >> 16;
		op->in_cvt.enable = operation.data[19] >> 24;
		op->in_cvt.offset = operation.data[20];
		op->out_cvt.scale = operation.data[21];
		op->out_cvt.truncate = operation.data[21] >> 16;
		op->out_cvt.enable = operation.data[21] >> 24;
		op->out_cvt.offset = operation.data[22];
	return 0;
}
int32_t conv_desc_to_op(struct dla_operation_container *operation, struct dla_conv_op_desc op)
{
	    operation->data[0] = ((uint32_t)op.conv_mode) |
	            		     ((uint32_t)op.data_reuse << 8) |
							 ((uint32_t)op.weight_reuse << 16) |
							 ((uint32_t)op.skip_data_rls << 24);
		operation->data[1] = ((uint32_t)op.skip_weight_rls) |
		 	 	 	 	 	 ((uint32_t)op.reserved0 << 8) |
		 	 	 	 	 	 ((uint32_t)op.entry_per_slice << 16);
		operation->data[2] = ((uint32_t)op.data_format) |
							 ((uint32_t)op.pixel_mapping << 8) |
							 ((uint32_t)op.fetch_grain << 16);
		operation->data[3] = ((uint32_t)op.reserved_b[0]) |
							 ((uint32_t)op.reserved_b[1] << 8) |
							 ((uint32_t)op.reserved_b[2] << 16) |
							 ((uint32_t)op.reserved_b[3] << 24);
		operation->data[4] = ((uint32_t)op.reserved_b[4]) |
							 ((uint32_t)op.reserved_b[5] << 8) |
							 ((uint32_t)op.reserved_b[6] << 16) |
							 ((uint32_t)op.reserved_b[7] << 24);
		operation->data[5] = ((uint32_t)op.batch) |
							 ((uint32_t)op.weight_format << 8) |
							 ((uint32_t)op.data_bank << 16) |
							 ((uint32_t)op.weight_bank << 24);
		operation->data[6] = ((uint32_t)op.batch_stride);
		operation->data[7] = ((uint32_t)op.post_extension) |
							 ((uint32_t)op.pixel_override << 8) |
							 ((uint32_t)op.release << 16);
		operation->data[8] = ((uint32_t)op.input_width_csc) |
						     ((uint32_t)op.input_height_csc << 16);
		operation->data[9] = ((uint32_t)op.input_channel_csc) |
							 ((uint32_t)op.kernel_width_csc << 16);
		operation->data[10] =((uint32_t)op.kernel_height_csc) |
							 ((uint32_t)op.kernel_channel_csc << 16);
		operation->data[11] =((uint32_t)op.input_width_cmac) |
							 ((uint32_t)op.input_height_cmac << 16);
		operation->data[12] = (uint32_t)op.bytes_per_kernel;
		operation->data[13] =((uint32_t)op.mean_ry) |
							 ((uint32_t)op.mean_gu << 16);
		operation->data[14] =((uint32_t)op.mean_bv) |
							 ((uint32_t)op.mean_ax << 16);
		operation->data[15] =((uint32_t)op.mean_format) |
							 ((uint32_t)op.conv_stride_x << 8) |
							 ((uint32_t)op.conv_stride_y << 16) |
							 ((uint32_t)op.pad_x_left << 24);
		operation->data[16] =((uint32_t)op.pad_x_right) |
							 ((uint32_t)op.pad_y_top << 8) |
							 ((uint32_t)op.pad_y_bottom << 16) |
							 ((uint32_t)op.dilation_x << 24);
		operation->data[17] =((uint32_t)op.dilation_y) |
							 ((uint32_t)op.reserved2[0] << 8) |
							 ((uint32_t)op.reserved2[1] << 16) |
							 ((uint32_t)op.pra_truncate << 24);
		operation->data[18] =((uint32_t)op.in_precision) |
							 ((uint32_t)op.out_precision << 8) |
							 ((uint32_t)op.pad_val << 16);
		operation->data[19] =((uint32_t)op.in_cvt.scale) |
							 ((uint32_t)op.in_cvt.truncate << 16) |
							 ((uint32_t)op.in_cvt.enable << 24);
		operation->data[20] = (uint32_t)op.in_cvt.offset;
		operation->data[21] =((uint32_t)op.out_cvt.scale) |
							 ((uint32_t)op.out_cvt.truncate << 16) |
							 ((uint32_t)op.out_cvt.enable << 24);
		operation->data[22] = (uint32_t)op.out_cvt.offset;

	return 0;
}
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
int32_t op_desc_to_rubik(struct dla_operation_container operation, struct dla_rubik_op_desc *op)
{
	// 以32位分组。
		op->mode = operation.data[0];
		op->precision = operation.data[0] >> 8;
		op->stride_x = operation.data[0] >> 16;
		op->stride_y = operation.data[0] >> 24;
	return 0;

}
int32_t rubik_desc_to_op(struct dla_operation_container *operation, struct dla_rubik_op_desc op)
{
	operation->data[0] = ((uint32_t)op.mode) |
						 ((uint32_t)op.precision << 8) |
						 ((uint32_t)op.stride_x << 16) |
						 ((uint32_t)op.stride_y << 24);
	return 0;
}

int32_t surface_desc_to_conv(struct dla_surface_container surface, struct dla_conv_surface_desc *surf)
{
		surf->weight_data.type = surface.data[0];
		surf->weight_data.address = surface.data[0] >> 16;
		surf->weight_data.offset = surface.data[1];
		surf->weight_data.size = surface.data[2];
		surf->weight_data.width = surface.data[3];
		surf->weight_data.height = surface.data[3] >> 16;
		surf->weight_data.channel = surface.data[4];
		surf->weight_data.reserved0 = surface.data[4] >> 16;
		surf->weight_data.line_stride = surface.data[5];
		surf->weight_data.surf_stride = surface.data[6];
		surf->weight_data.plane_stride = surface.data[7];

		surf->wmb_data.type = surface.data[8];
		surf->wmb_data.address = surface.data[8] >> 16;
		surf->wmb_data.offset = surface.data[9];
		surf->wmb_data.size = surface.data[10];
		surf->wmb_data.width = surface.data[11];
		surf->wmb_data.height = surface.data[11] >> 16;
		surf->wmb_data.channel = surface.data[12];
		surf->wmb_data.reserved0 = surface.data[12] >> 16;
		surf->wmb_data.line_stride = surface.data[13];
		surf->wmb_data.surf_stride = surface.data[14];
		surf->wmb_data.plane_stride = surface.data[15];

		surf->wgs_data.type = surface.data[16];
		surf->wgs_data.address = surface.data[16] >> 16;
		surf->wgs_data.offset = surface.data[17];
		surf->wgs_data.size = surface.data[18];
		surf->wgs_data.width = surface.data[19];
		surf->wgs_data.height = surface.data[19] >> 16;
		surf->wgs_data.channel = surface.data[20];
		surf->wgs_data.reserved0 = surface.data[20] >> 16;
		surf->wgs_data.line_stride = surface.data[21];
		surf->wgs_data.surf_stride = surface.data[22];
		surf->wgs_data.plane_stride = surface.data[23];

		surf->src_data.type = surface.data[24];
		surf->src_data.address = surface.data[24] >> 16;
		surf->src_data.offset = surface.data[25];
		surf->src_data.size = surface.data[26];
		surf->src_data.width = surface.data[27];
		surf->src_data.height = surface.data[27] >> 16;
		surf->src_data.channel = surface.data[28];
		surf->src_data.reserved0 = surface.data[28] >> 16;
		surf->src_data.line_stride = surface.data[29];
		surf->src_data.surf_stride = surface.data[30];
		surf->src_data.plane_stride = surface.data[31];

		surf->dst_data.type = surface.data[32];
		surf->dst_data.address = surface.data[32] >> 16;
		surf->dst_data.offset = surface.data[33];
		surf->dst_data.size = surface.data[34];
		surf->dst_data.width = surface.data[35];
		surf->dst_data.height = surface.data[35] >> 16;
		surf->dst_data.channel = surface.data[36];
		surf->dst_data.reserved0 = surface.data[36] >> 16;
		surf->dst_data.line_stride = surface.data[37];
		surf->dst_data.surf_stride = surface.data[38];
		surf->dst_data.plane_stride = surface.data[39];

	return 0;
}
int32_t conv_desc_to_surface(struct dla_surface_container *surface, struct dla_conv_surface_desc surf)
{
	surface->data[0] = ((uint32_t)surf.weight_data.type) |
					   ((uint32_t)surf.weight_data.address << 16);
	surface->data[1] =  (uint32_t)surf.weight_data.offset;
	surface->data[2] =  (uint32_t)surf.weight_data.size;
	surface->data[3] = ((uint32_t)surf.weight_data.width) |
					   ((uint32_t)surf.weight_data.height << 16);
	surface->data[4] = ((uint32_t)surf.weight_data.channel) |
					   ((uint32_t)surf.weight_data.reserved0 << 16);
	surface->data[5] =  (uint32_t)surf.weight_data.line_stride;
	surface->data[6] =  (uint32_t)surf.weight_data.surf_stride;
	surface->data[7] =  (uint32_t)surf.weight_data.plane_stride;

	surface->data[8] = ((uint32_t)surf.wmb_data.type) |
					   ((uint32_t)surf.wmb_data.address << 16);
	surface->data[9] =  (uint32_t)surf.wmb_data.offset;
	surface->data[10] = (uint32_t)surf.wmb_data.size;
	surface->data[11] =((uint32_t)surf.wmb_data.width) |
					   ((uint32_t)surf.wmb_data.height << 16);
	surface->data[12] =((uint32_t)surf.wmb_data.channel) |
					   ((uint32_t)surf.wmb_data.reserved0 << 16);
	surface->data[13] = (uint32_t)surf.wmb_data.line_stride;
	surface->data[14] = (uint32_t)surf.wmb_data.surf_stride;
	surface->data[15] = (uint32_t)surf.wmb_data.plane_stride;

	surface->data[16] =((uint32_t)surf.wgs_data.type) |
					   ((uint32_t)surf.wgs_data.address << 16);
	surface->data[17] = (uint32_t)surf.wgs_data.offset;
	surface->data[18] = (uint32_t)surf.wgs_data.size;
	surface->data[19] =((uint32_t)surf.wgs_data.width) |
					   ((uint32_t)surf.wgs_data.height << 16);
	surface->data[20] =((uint32_t)surf.wgs_data.channel) |
					   ((uint32_t)surf.wgs_data.reserved0 << 16);
	surface->data[21] = (uint32_t)surf.wgs_data.line_stride;
	surface->data[22] = (uint32_t)surf.wgs_data.surf_stride;
	surface->data[23] = (uint32_t)surf.wgs_data.plane_stride;

	surface->data[24] =((uint32_t)surf.src_data.type) |
					   ((uint32_t)surf.src_data.address << 16);
	surface->data[25] = (uint32_t)surf.src_data.offset;
	surface->data[26] = (uint32_t)surf.src_data.size;
	surface->data[27] =((uint32_t)surf.src_data.width) |
					   ((uint32_t)surf.src_data.height << 16);
	surface->data[28] =((uint32_t)surf.src_data.channel) |
					   ((uint32_t)surf.src_data.reserved0 << 16);
	surface->data[29] = (uint32_t)surf.src_data.line_stride;
	surface->data[30] = (uint32_t)surf.src_data.surf_stride;
	surface->data[31] = (uint32_t)surf.src_data.plane_stride;

	surface->data[32] =((uint32_t)surf.dst_data.type) |
					   ((uint32_t)surf.dst_data.address << 16);
	surface->data[33] = (uint32_t)surf.dst_data.offset;
	surface->data[34] = (uint32_t)surf.dst_data.size;
	surface->data[35] =((uint32_t)surf.dst_data.width) |
					   ((uint32_t)surf.dst_data.height << 16);
	surface->data[36] =((uint32_t)surf.dst_data.channel) |
					   ((uint32_t)surf.dst_data.reserved0 << 16);
	surface->data[37] = (uint32_t)surf.dst_data.line_stride;
	surface->data[38] = (uint32_t)surf.dst_data.surf_stride;
	surface->data[39] = (uint32_t)surf.dst_data.plane_stride;
	return 0;
}
int32_t surface_desc_to_rubik(struct dla_surface_container surface, struct dla_rubik_surface_desc *surf)
{
		surf->src_data.type = surface.data[0];
		surf->src_data.address = surface.data[0] >> 16;
		surf->src_data.offset = surface.data[1];
		surf->src_data.size = surface.data[2];
		surf->src_data.width = surface.data[3];
		surf->src_data.height = surface.data[3] >> 16;
		surf->src_data.channel = surface.data[4];
		surf->src_data.reserved0 = surface.data[4] >> 16;
		surf->src_data.line_stride = surface.data[5];
		surf->src_data.surf_stride = surface.data[6];
		surf->src_data.plane_stride = surface.data[7];
	return 0;
}
int32_t rubik_desc_to_surface(struct dla_surface_container *surface, struct dla_rubik_surface_desc surf)
{
	    surface->data[0] =((uint32_t)surf.src_data.type) |
						   ((uint32_t)surf.src_data.address << 16);
		surface->data[1] = (uint32_t)surf.src_data.offset;
		surface->data[2] = (uint32_t)surf.src_data.size;
		surface->data[3] =((uint32_t)surf.src_data.width) |
						   ((uint32_t)surf.src_data.height << 16);
		surface->data[4] =((uint32_t)surf.src_data.channel) |
						   ((uint32_t)surf.src_data.reserved0 << 16);
		surface->data[5] = (uint32_t)surf.src_data.line_stride;
		surface->data[6] = (uint32_t)surf.src_data.surf_stride;
		surface->data[7] = (uint32_t)surf.src_data.plane_stride;
	return 0;
}
int32_t dla_dep_graph_read(uint64_t *src,struct dla_common_op_desc *dst,uint64_t offset)
{
#pragma HLS INTERFACE m_axi depth=64 port=src offset = slave

	int i=0;
	uint64_t buf[32];

	memcpy(buf,src + (offset * sizeof(struct dla_common_op_desc)) / 8,sizeof(struct dla_common_op_desc));

				dst->index = *(buf);
				dst->roi_index = *(buf) >> 16;
				dst->op_type =  *(buf) >> 24;
				dst->dependency_count = *(buf) >> 32;
				dst->reserved0[0] = *(buf) >> 40;
				dst->reserved0[1] = *(buf) >> 48;
				dst->reserved0[2] = *(buf) >> 56;
				for (i = 0; i < 6; i ++)
				{
					dst->consumers[i].index = *(buf + (i / 2 + 1)) >> (32 * (i % 2) + 0);
					dst->consumers[i].event = *(buf + (i / 2 + 1)) >> (32 * (i % 2) + 16);
					dst->consumers[i].res = *(buf + (i / 2 + 1)) >> (32 * (i % 2) + 24);
			    }
				dst->fused_parent.index = *(buf + 4);
				dst->fused_parent.event = *(buf + 4) >> 16;
				dst->fused_parent.res = *(buf + 4) >> 24;
		return 0;
}
int32_t dla_dep_graph_write(uint64_t *dst,struct dla_common_op_desc *src,uint64_t offset)
{
#pragma HLS INTERFACE m_axi depth=64 port=dst offset = slave

	int i=0;
	uint64_t buf[32] = { 0 };

	*(buf) = ((uint64_t)src->index) |
			 ((uint64_t)(src->roi_index) << 16) |
			 ((uint64_t)(src->op_type) << 24) |
			 ((uint64_t)(src->dependency_count) << 32) |
			 ((uint64_t)(src->reserved0[0]) << 40) |
			 ((uint64_t)src->reserved0[1] << 48) |
			 ((uint64_t)src->reserved0[2] << 56);

	for (i = 0; i < 6; i ++)
	{
		*(buf + (i / 2 + 1)) = *(buf + (i / 2 + 1)) |
							   ((uint64_t)src->consumers[i].index << (32 * (i % 2) + 0)) |
							   ((uint64_t)src->consumers[i].event << (32 * (i % 2) + 16)) |
							   ((uint64_t)src->consumers[i].res << (32 * (i % 2) + 24)) ;

	}

	*(buf + 4) = ((uint64_t)src->fused_parent.index) |
			     ((uint64_t)src->fused_parent.event << 16) |
				 ((uint64_t)src->fused_parent.res << 24);

	memcpy(dst + (offset * sizeof(struct dla_common_op_desc)) / 8,buf,sizeof(struct dla_common_op_desc));

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
int32_t dla_surfcae_config_read(uint32_t *src,struct dla_surface_container *dst,uint64_t offset)
{
#pragma HLS INTERFACE m_axi port=src depth=64 offset = slave

	int i;
	uint32_t buf[161];

	memcpy(buf,src + (offset * sizeof(struct dla_surface_container)) / 8,sizeof(struct dla_surface_container));

	for(i = 0; i < 161; i++)
	{
		*(dst->data + i) = *(buf + i);
	}

	return 0;
}
int32_t dla_surface_config_write(uint32_t *dst,struct dla_surface_container *src,uint64_t offset)
{
#pragma HLS INTERFACE m_axi depth=64 port=dst offset = slave

	int i;
	uint32_t buf[161];

	for(i = 0; i < 161; i++)
		{
			*(buf + i) = *(src->data + i);
		}

	memcpy(dst + (offset * sizeof(struct dla_surface_container)) / 8,buf,sizeof(struct dla_surface_container));

	return 0;
}
void reg_write(nvdla_base_t nvdla_base, uint32_t addr, uint32_t reg)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

	nvdla_base[addr] = reg;
}
uint32_t reg_read(nvdla_base_t nvdla_base, const uint32_t addr)
{
#pragma HLS INTERFACE m_axi port=addr depth=32 offset = slave

	return nvdla_base[addr];
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

