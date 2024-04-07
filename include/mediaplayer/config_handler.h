#pragma once

#define CONFIG_MAX_SIZE 300

// encoders
#define VIDEO_ENCODER_X264 0x00 // cpu
#define VIDEO_ENCODER_NVENC 0x01 // nvidia
#define VIDEO_ENCODER_QSV 0x02 // intel
#define VIDEO_ENCODER_AMF 0x03 // amd

// decoders
#define VIDEO_DECODER_H264 0x00 // cpu
#define VIDEO_DECODER_CUVID 0x01 // nvidia
#define VIDEO_DECODER_QSV 0x02 // intel
// ******* there's no decoder for amd gpu ******

// video process performance level
#define VIDEO_PROCESS_PERF_LOWER 0x00 // low resource usage (for low-end pc)
#define VIDEO_PROCESS_PERF_LOW 0x01
#define VIDEO_PROCESS_PERF_MID 0x02
#define VIDEO_PROCESS_PERF_HIGH 0x03
#define VIDEO_PROCESS_PERF_HIGHER 0x04 // high resource usage (for high-end pc)

// convert mode
#define VIDEO_CONVERT_MODE_NONE 0x00
#define VIDEO_CONVERT_MODE_AT_PLAY 0x01
#define VIDEO_CONVERT_MODE_AT_SERVER_START 0x02
#define VIDEO_CONVERT_MODE_AT_BACKGROUND 0x03

// video process mode
#define VIDEO_PROCESS_MODE_NONE 0x00
#define VIDEO_PROCESS_MODE_REALTIME 0x01
#define VIDEO_PROCESS_MODE_CONVERT_TO_IMG 0x02

// video to img fmt
#define VIDEO_IMG_OUTPUT_FMT_RAW 0x00 // fastest but more storage
#define VIDEO_IMG_OUTPUT_FMT_PNG 0x01
#define VIDEO_IMG_OUTPUT_FMT_JPEG 0x02

struct config_file {
	long size;
	char *data;
};

int config_open_file(struct config_file *in_config_file, char *in_filename);
int config_read(struct config_file *in_config_file, char *in_valname, char **out_val);