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
#include <linux/mman.h>
#include <libv4l2.h>

#define print printf

struct l_buffer {
	void *	start;
	size_t	length;
};

int main(int argc, char **argv)
{
	int fd = -1;
	struct l_buffer *pbuffer = NULL;
	int width = 640;int height = 480;
	struct v4l2_capability v4l2_cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req_buf;
	struct v4l2_buffer pbuf;

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
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
	{
		print("ioctl VIDIOC_G_FMT error\n");
	}
	else
	{
		print("type:0x%x\n", fmt.type);
		print("width:%d, height:%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
		width = fmt.fmt.pix.width;
		height = fmt.fmt.pix.height;
		print("pixelformat:0x%x\n", fmt.fmt.pix.pixelformat);
		print("line_size:%d, total_size:%d\n", fmt.fmt.pix.bytesperline, fmt.fmt.pix.sizeimage);
		print("colorspace:%d\n", fmt.fmt.pix.colorspace);
	}
	// request 3 mmap buffers
	memset(&req_buf, 0, sizeof(struct v4l2_requestbuffers));
	req_buf.count = 3;
	req_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req_buf.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req_buf) == -1)
	{
		print("ioctl VIDIOC_REQBUFS error\n");
	}

	// get buffer point
	memset(&pbuf, 0, sizeof(struct v4l2_buffer));
	pbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_QUERYBUF, &pbuf) == -1)
	{
		print("ioctl VIDIOC_QUERYBUF error\n");
	}
	else
	{
		print("index:%d\n", pbuf.index);
		print("bytesused:%d\n", pbuf.bytesused);
		print("mem_addr:%p\n", pbuf.memory);
	}

	// mmap memory
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		pbuffer[i].start =
					mmap (NULL /* start anywhere */,
					height*width*2,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					fd, offset);

		if (MAP_FAILED == pbuffer[i].start)
		{
			print("mmap failed\n");
		}
	}
	// select capture video0
	if (ioctl(fd, VIDIOC_DQBUF, &pbuf) == -1)
	{
		print("ioctl VIDIOC_QDBUF error\n");
	}
	else
	{
		print("index:%d\n", pbuf.index);
		print("bytesused:%d\n", pbuf.bytesused);
		print("mem_addr:%p\n", pbuf.memory);
	}

	// release buffer
	if (ioctl(fd, VIDIOC_QBUF, &pbuf) == -1)
	{
		print("ioctl VIDIOC_QBUF error\n");
	}
	else
	{
		print("index:%d\n", pbuf.index);
		print("bytesused:%d\n", pbuf.bytesused);
		print("mem_addr:%p\n", pbuf.memory);
	}


	// close file
	v4l2_close(fd);

	return 0;
}

