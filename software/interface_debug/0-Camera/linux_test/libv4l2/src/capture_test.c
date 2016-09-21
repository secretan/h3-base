/*
 * capture_test.c
 *
 *  Created on: 2016年9月21日
 *      Author: 小明
 */

#include "v4l2_capture.h"

int main(int argc, char **argv)
{
	capture_init();
	capture_start();

	while(1)
		capture_mainloop();

	catputre_stop();
	capture_deinit();

	return 0;
}

