/*
 * v4l2_capture.h
 *
 *  Created on: 2016年9月21日
 *      Author: 小明
 */

#ifndef SRC_V4L2_CAPTURE_H_
#define SRC_V4L2_CAPTURE_H_


int capture_init();
int capture_deinit();

int capture_start();
int capture_stop();

void capture_mainloop();

#endif /* SRC_V4L2_CAPTURE_H_ */
