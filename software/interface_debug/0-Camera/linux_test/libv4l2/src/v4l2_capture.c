/*
 * v4l2_capture.c
 *
 *  Created on: 2016年9月21日
 *      Author: 小明
 */

#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <errno.h>
#include <libv4l2.h>

struct buffer
{
	void * start;
	size_t length;
};

#define print printf
#define BUFFER_NUM 3

int v4l2_fd = -1;
int width = 640;
int height = 480;
struct buffer *v4l2_buf = NULL;
uint8_t *frame = NULL;


int capture_init()
{
	int i;
	struct v4l2_format v4l2_fmt;
	struct v4l2_capability v4l2_cap;
	struct v4l2_requestbuffers v4l2_reqbuf;
	struct v4l2_buffer v4l2_buff;

	// open
	v4l2_fd = v4l2_open("/dev/video0", O_RDWR);
	if (v4l2_fd < 0)
	{
		print("v4l2_open error\n");
		return -1;
	}
	// detect camera
	if (v4l2_ioctl(v4l2_fd, VIDIOC_QUERYCAP, &v4l2_cap) == -1)
	{
		print("ioctl VIDIOC_QUERYCAP error\n");
		return -2;
	}
	if ((v4l2_cap.capabilities && V4L2_CAP_VIDEO_CAPTURE)
			== V4L2_CAP_VIDEO_CAPTURE)
	{
		print("video0 can capture video\n");
		// get source format
		memset(&v4l2_fmt, 0, sizeof(struct v4l2_format));
		v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (v4l2_ioctl(v4l2_fd, VIDIOC_G_FMT, &v4l2_fmt) == -1)
		{
			print("v4l2_ioctl VIDIOC_G_FMT error\n");
			return -2;
		}
		width = v4l2_fmt.fmt.pix.width;
		height = v4l2_fmt.fmt.pix.height;
		frame = (uint8_t *)malloc(v4l2_fmt.fmt.pix.sizeimage);
	}
	// request 3 mmap buffers
	memset(&v4l2_reqbuf, 0, sizeof(struct v4l2_requestbuffers));
	v4l2_reqbuf.count = BUFFER_NUM;
	v4l2_reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_reqbuf.memory = V4L2_MEMORY_MMAP;
	if (v4l2_ioctl(v4l2_fd, VIDIOC_REQBUFS, &v4l2_reqbuf) == -1)
	{
		print("ioctl VIDIOC_REQBUFS error\n");
	}
	// malloc 3 buffers
	v4l2_buf = (struct buffer*)malloc(sizeof(struct buffer)*BUFFER_NUM);
	for (i = 0; i < BUFFER_NUM; i++)
	{
		memset(&v4l2_buff, 0, sizeof(struct v4l2_buffer));
		v4l2_buff.index = i;
		v4l2_buff.memory = V4L2_MEMORY_MMAP;
		v4l2_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (v4l2_ioctl(v4l2_fd, VIDIOC_QUERYBUF, &v4l2_buff) ==  -1);
		{
			print ("ioctl VIDIOC_QUERYBUF %d error\n", i);
		}

		// memory map
		v4l2_buf[i].start =
				v4l2_mmap(NULL /* start anywhere */, v4l2_buff.length,
				PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */, v4l2_fd, v4l2_buff.m.offset);
		if (v4l2_buf[i].start == MAP_FAILED)
		{
			print ("v4l2_mmap error %d\n", i);
			v4l2_buf[i].length = 0;
		}
		else
		{
			v4l2_buf[i].length = v4l2_buff.length;
		}

		// request buffer ready to get data
		if (v4l2_buf[i].length)
		{
			if (v4l2_ioctl(v4l2_fd, VIDIOC_QBUF, &v4l2_buff) == -1)
			{
				print ("ioctl VIDIOC_QBUF error\n");
			}
		}
	}

	return 0;
}

int capture_deinit()
{
	int i;

	for (i = 0; i < BUFFER_NUM; i++)
	{
		v4l2_munmap(v4l2_buf[i].start, v4l2_buf[i].length);
	}
	free(v4l2_buf);
	close(v4l2_fd);

	return 0;
}

int capture_start()
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	return v4l2_ioctl(v4l2_fd, VIDIOC_STREAMON, &type);
}

int capture_stop()
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	return v4l2_ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
}

int capture_frame()
{
	struct v4l2_buffer buf;

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == v4l2_ioctl (v4l2_fd, VIDIOC_DQBUF, &buf))
	{
		switch (errno)
		{
		case EAGAIN:
			return 0;

		case EIO:
			if (!(buf.flags & (V4L2_BUF_FLAG_QUEUED | V4L2_BUF_FLAG_DONE)))
			{
				if (v4l2_ioctl(v4l2_fd, VIDIOC_QBUF, &buf) == -1)
				{
					return 0;
				}
			}
			return 0;

		default:
			/* display the error and stop processing */
			perror("VIDIOC_DQBUF");
			return 1;
		}
	}

	assert(buf.index < 3);

	memcpy(frame,
			v4l2_buf[buf.index].start,
			v4l2_buf[buf.index].length);

	//printf("got data in buff %d, len=%d, flags=0x%X, seq=%d, used=%d)\n",
	//	  buf.index, buf.length, buf.flags, buf.sequence, buf.bytesused);

	if (-1 == v4l2_ioctl (v4l2_fd, VIDIOC_QBUF, &buf))
	{
		perror("VIDIOC_QBUF");
	}

	return 0;
}

void capture_mainloop()
{
	unsigned int count;

	count = 1;

	while (count-- > 0)
	{
		for (;;)
		{
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(v4l2_fd, &fds);

			/* Timeout. */
			tv.tv_sec = 10;
			tv.tv_usec = 0;

			r = select(v4l2_fd, &fds, NULL, NULL, &tv);

			if (-1 == r)
			{
				if (EINTR == errno)
					continue;
				perror("select");
			}

			if (0 == r)
			{
				fprintf(stderr, "select timeout\n");

				/* end the infinite loop */
				break;
			}

			if (capture_frame())
				break;
		}
	}
}


