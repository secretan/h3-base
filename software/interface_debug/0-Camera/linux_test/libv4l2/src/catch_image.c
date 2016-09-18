/*
 * catch_image.c
 *
 *  Created on: 2016年9月18日
 *      Author: 小明
 */

#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

#define print printf

int main(int argc, char **argv)
{
	int fd = -1;
	struct v4l2_capability v4l2_cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req_buf;

	// open file
	fd = v4l2_open("/dev/video0", O_RDWR);
	if (fd < 0)
	{
		print("v4l2 open error\n");
		return -1;
	}

	// detect camera
	if (ioctl(fd, VIDIOC_QUERYCAP, &v4l2_cap) == -1)
	{
		print("ioctl VIDIOC_QUERYCAP error\n");
		return -2;
	}
	if ((v4l2_cap.capabilities && V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
	{
		print("video0 can capture video\n");
	}
	if ((v4l2_cap.capabilities && V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
	{
		print("video0 can set video streaming\n");
	}
	// enum device
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == -1)
	{
		print("ioctl VIDIOC_ENUM_FMT error\n");
	}
	print("capture image format:%s\n", fmtdesc.description);
	// get source format
	memset(&fmt, 0, sizeof(struct v4l2_format));
	if (ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
	{
		print("ioctl VIDIOC_G_FMT error\n");
	}
	else
	{
		print("type:0x%x\n", fmt.type);
		print("width:%d, height:%d\n", fmt.fmt.width, fmt.fmt.height);
		print("pixelformat:0x%x\n", fmt.fmt.pixelformat);
		print("line_size:%d, total_size:%d\n", fmt.fmt.bytesperline, fmt.fmt.sizeimage);
		print("colorspace:%d\n", fmt.fmt.colorspace);
	}
	// request 3 mmap buffers
	memset(&req_buf, 0, sizeof(struct v4l2_requestbuffers));
	req_buf.count = 3;
	req_buf.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req_buf) == -1)
	{
		print("ioctl VIDIOC_REQBUFS error\n");
	}

	// select capture video0




	// close file
	v4l2_close(fd);

	return 0;
}

