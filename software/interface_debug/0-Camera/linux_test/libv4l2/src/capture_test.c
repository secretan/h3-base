/*
 * capture_test.c
 *
 *  Created on: 2016��9��21��
 *      Author: С��
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

