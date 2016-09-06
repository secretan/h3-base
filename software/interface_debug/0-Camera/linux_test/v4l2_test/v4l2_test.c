/*************************************************************************
	> File Name: v4l2_test.c
	> Author: 
	> Mail: 
	> Created Time: Mon 05 Sep 2016 09:31:36 AM EDT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>            

#include <fcntl.h>             
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>         
#include <time.h>
#include <linux/videodev2.h>

int cam_fd = 0;
int out_fd = 0;

struct buffer {
    void *start;
    size_t length;
};

// video buffer
struct buffer *buffers = NULL;

static int read_frame()
{
    
}

int camera_init()
{

}



