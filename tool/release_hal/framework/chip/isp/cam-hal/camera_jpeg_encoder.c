#include "plat_types.h"
#include "ui_mem_api.h"
#ifdef ENABLE_SOFT_JPEG
#include "jpeglib.h"
#endif
#include "../online_drv/cam_pipeline.h"
#include "camera_jpeg_encoder.h"
#include "../camera_common_log.h"
#include "../offline_drv/cpp/afbc_drv.h"
#include "../offline_drv/jpeg_dma.h"
#include "../offline_drv/jpeg_drv.h"
#include "cp_include.h"
#include "../online_drv/pipeline/cam_pipeline_reg.h"



#define CC_JPEG_HEADER_SIZE 597

#ifdef ENABLE_SOFT_JPEG
void jpeg_error_exit(j_common_ptr cinfo)
{

    CAMLOGE("soft jpeg encoder failed!");
}

unsigned int CameraSoftJpegEncode(CCameraSofJpegInfo *inJpegInfo, CCameraSofJpegInfo *outJpegInfo, int quality, int orientation)
{
    struct jpeg_compress_struct cinfo;
    unsigned int jpeg_size = 0;
    int encQuality = 95;
//    int encOrientation = 0;
    unsigned char *outData = (void*)0;
    unsigned int i, j, k, index = 0;
    JSAMPROW y[16] = {0}, cb[8] = {0}, cr[8] = {0};
    JSAMPARRAY data[3];
    unsigned int image_width, image_height;
//    struct timespec timeIn;
//    struct timespec timeOut;
//    uint64_t timeCostms = 0;

//    clock_gettime( CLOCK_MONOTONIC, &timeIn );

    struct jpeg_error_mgr jerr;
    jerr.error_exit = jpeg_error_exit;

    image_width = inJpegInfo->width;
    image_height = inJpegInfo->height;

    data[0] = y;
    data[1] = cb;
    data[2] = cr;

    for(i = 0; i < 8; i++) {
        cb[i] = (JSAMPROW)UI_MALLOC(sizeof(JSAMPROW) * image_width / 2);
        cr[i] = (JSAMPROW)UI_MALLOC(sizeof(JSAMPROW) * image_width / 2);
		if (!cb[i] || !cr[i]) {
			CAMLOGE("UI_MALLOC for JSAMPROW failed!");
			goto Error_exit;
		}
	}

#if 0
	outData = (unsigned char*)UI_MALLOC(4096);
	if (!outData) {
		CAMLOGE("UI_MALLOC for outData failed!");
		goto Error_exit;
	}
	jpeg_size = 4096;
#endif

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    jpeg_mem_dest(&cinfo, &outData, (unsigned long*)&jpeg_size);

    cinfo.image_width = image_width; /* image width and height, in pixels */
    cinfo.image_height = image_height;
    cinfo.input_components = 3; /* # of color components per pixel */
    cinfo.in_color_space = JCS_YCbCr; /* colorspace of input image */

    jpeg_set_defaults(&cinfo);

    cinfo.raw_data_in = TRUE;
    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 2;
    cinfo.comp_info[1].h_samp_factor = 1;
    cinfo.comp_info[1].v_samp_factor = 1;
    cinfo.comp_info[2].h_samp_factor = 1;
    cinfo.comp_info[2].v_samp_factor = 1;

    //cinfo.jpeg_width = jpegImage->imageInfo.size.width;
    //cinfo.jpeg_height = jpegImage->imageInfo.size.height;

#if 0
    switch(quality) {
        case CC_JPEG_QUALITY_LOW:
            encQuality = 65;
            break;
        case CC_JPEG_QUALITY_NORMAL:
            encQuality = 75;
            break;
        case CC_JPEG_QUALITY_FINE:
            encQuality = 85;
            break;
        case CC_JPEG_QUALITY_SUPERFINE:
            encQuality = 95;
            break;
        default:
            encQuality = 95;
            break;
    }
#endif
/*
    switch(orientation) {
        case 0:
            encOrientation = 0;
            break;
        case 90:
            encOrientation = 270;
            break;
        case 180:
            encOrientation = 180;
            break;
        case 270:
            encOrientation = 90;
            break;
    }
*/
    //FIXME: not support downscaling
    if(outJpegInfo->width <= 480 &&
       outJpegInfo->width != inJpegInfo->width &&
       outJpegInfo->height != inJpegInfo->height) {
        //thumbnail quality should be lower
        CAMLOGW("FIXME: not support downscaling, maybe crash");
        encQuality -= 20;
    }

    jpeg_set_quality(&cinfo, encQuality, TRUE);

  //  jpeg_set_rotation((j_common_ptr)&cinfo, encOrientation);

    cinfo.dct_method = JDCT_FASTEST;

    jpeg_start_compress(&cinfo, TRUE);

    for (j = 0; j < image_height; j += 16) {
        for (i = 0; i < 16; i++) {
            if((i + j) >= image_height) break;
            y[i] = (JSAMPROW)inJpegInfo->addr[0] + inJpegInfo->width * (i + j); //FIXME: use plane width as stride
            index = 0;
            if (i%2 == 0) {
                for(k = 0; (k+1) < image_width; k += 2) {
                   *(cb[i/2] + index) = *((JSAMPROW)inJpegInfo->addr[1] + inJpegInfo->width * (i + j) / 2 + k);  //FIXME: use plane width as stride
                   *(cr[i/2] + index) = *((JSAMPROW)inJpegInfo->addr[1] + inJpegInfo->width * (i + j) / 2 + k + 1);  //FIXME: use plane width as stride
                    index++;
                }
            }
        }
        jpeg_write_raw_data(&cinfo, data, 16);
    }

    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);

    if(jpeg_size > outJpegInfo->bufferLen) {
        CAMLOGE("jpeg size(%d) is larger than allocated buffer(%d), jpeg encoder failed", jpeg_size, outJpegInfo->bufferLen);
        return 0;
    }

    memcpy((void*)outJpegInfo->addr[0], outData, jpeg_size);

Error_exit:
    for(i = 0; i < 8; i++) {
		if (cb[i])
			UI_FREE(cb[i]);
		if (cr[i])
			UI_FREE(cr[i]);
    }

    if(outData) {
		UI_FREE(outData);
	}

 //   clock_gettime( CLOCK_MONOTONIC, &timeOut );

 //   timeCostms = ((int64_t)timeOut.tv_sec * 1000000LL + timeOut.tv_nsec / 1000LL - ((int64_t)timeIn.tv_sec * 1000000LL + timeIn.tv_nsec / 1000LL)) / 1000LL;

 //   CAMLOGI("jpeg encode cost %ju ms", timeCostms);
	CAMLOGI("soft jpeg encode done!");
 
    return jpeg_size;
}
#endif

/*
 * @brief only support encoding for NV12, 16byte-aligned
 *
 * @param quality-JPEG_QUALITY_LOW at default
 * @param orientation-rotate with jdma which get image from psram offline
 *
 * return maybe negetive
 */
int CameraCodaJpegEncode(CCameraSofJpegInfo *inJpegInfo, CCameraSofJpegInfo *outJpegInfo, int quality, int orientation)
{
	int ret = 0;
	int encQuality;
	uint32_t rot;
	uint32_t in_w, in_h, ot_w, ot_h;
	uint32_t output_start_addr, output_end_addr;
	int bitstream_size = 0;

	in_w = inJpegInfo->width;
	in_h = inJpegInfo->height;
	output_start_addr = outJpegInfo->addr[0];

	if (quality >= 95)
		encQuality = JPEG_QUALITY_SUPERFINE;
	else if (quality >= 85)
		encQuality = JPEG_QUALITY_FINE;
	else if (quality >= 75)
		encQuality = JPEG_QUALITY_NORMAL;
	else
		encQuality = JPEG_QUALITY_LOW;

	switch (orientation) {
		case 0:
			rot = 0;
			ot_w = in_w;
			ot_h = in_h;
			break;
		case 90:
			rot = 90;
			ot_w = in_h;
			ot_h = in_w;
			break;
		case 180:
			rot = 180;
			ot_w = in_w;
			ot_h = in_h;
			break;
		case 270:
			rot = 270;
			ot_w = in_h;
			ot_h = in_w;
			break;
		default:
			CAMLOGE("invalid orientation %d", orientation);
			return 0;
	}

	if (inJpegInfo->addr[0] & 0x1F) {
		CAMLOGE("inJpeg addr[0x%08x] should 32Bytes algned!!!",
			inJpegInfo->addr[0]);
		return 0;
	}
	CacheInvalidateMemory((void*)output_start_addr, outJpegInfo->bufferLen);

	ret = cam_jpu_encopen(OFFLINE_ENC_MODE);
	if (ret < 0)
		return 0;

	jdma_cfg(in_w, in_h, in_w, rot, inJpegInfo->addr[0], inJpegInfo->addr[1]);
	jpeg_quality_set(encQuality);
	jpeg_enc_cfg(ot_w, ot_h, output_start_addr);
	jdma_trigger();

	output_end_addr = cam_jpu_encfinish();
	if (output_end_addr <= output_start_addr) {
		CAMLOGE("failed#######");
	} else {
		bitstream_size = output_end_addr - output_start_addr;
		CAMLOGD("done, bitstream size is 0x%x", bitstream_size);
	}

	cam_jpu_encclose();

	return bitstream_size;
}

/*
 * @brief only support online encoding for NV12,  pipeline2 dma stride request 128-aligned
 *           not support zoom and rotation
 *
 * @param quality-JPEG_QUALITY_LOW at default
 *
 * return maybe negetive
 */
int CameraOnlineJpegEncode(struct pipeline_param *pipeline, uint32_t output_start_addr, uint32_t size, int quality, int enablePreview)
{
	int ret = 0;
	int encQuality;
	uint32_t p2_dmad[3];
	uint32_t width, height, stride;
	uint32_t output_end_addr;
	int bitstream_size = 0;

	width = pipeline->pipeline_outw;
	height = pipeline->pipeline_outh;
	stride = pipeline->dma_stride_y;
	if (stride & 0x7F) {
		CAMLOGE("pipeline dma stride need 128 aligned, stride = %d", stride);
		return 0;
	}

	if (quality >= 95)
		encQuality = JPEG_QUALITY_SUPERFINE;
	else if (quality >= 85)
		encQuality = JPEG_QUALITY_FINE;
	else if (quality >= 75)
		encQuality = JPEG_QUALITY_NORMAL;
	else
		encQuality = JPEG_QUALITY_LOW;

	CacheInvalidateMemory((void*)output_start_addr, size);

	ret = cam_jpu_encopen(ONLINE_ENC_MODE);
	if (ret < 0)
		return 0;

	p2_dmad[0] = 0x7e700000;
	p2_dmad[1] = 0x7e700000 + stride * height;

	camafbc_test_enc_start(CAM_AFBC_ENC_SEL_JPEG_ONLY, width, height, 0, 0, 0, 0,
			       p2_dmad[0], 0, p2_dmad[1], 0);
	jpeg_quality_set(encQuality);
	jpeg_enc_cfg(width, height, output_start_addr);
	/* update p2 mac address for online jpeg */
	pipeline_update_mac_addr(PIPELINE_ID_CAP, p2_dmad, 2);

	cam_set_pipe_shadow_ready(PIPELINE_ID_CAP);
	if(enablePreview > 0) {
		cam_set_pipe_shadow_ready(PIPELINE_ID_PREV);
	}

	output_end_addr = cam_jpu_encfinish();
	if (output_end_addr <= output_start_addr) {
		CAMLOGE("failed#######");
	} else {
		bitstream_size = output_end_addr - output_start_addr;
		CAMLOGD("done, bitstream size is 0x%x", bitstream_size);
	}

	cam_jpu_encclose();

	return bitstream_size;	
}
