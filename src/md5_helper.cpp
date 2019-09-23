/*
 * Copyright 2018 SU Technology Ltd. All rights reserved.
 *
 * MIT License
 *
 * Copyright (c) 2018 SU Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Some parts of this file has been inspired and influenced from this link :
 * https://www.learnopencv.com/deep-learning-based-object-detection
 * -and-instance-segmentation-using-mask-r-cnn-in-opencv-python-c/
 *
 * md5_helper.cpp
 *
 *  Created on: 13 Sep 2019
 *      Author: root
 */

#include "md5_helper.h"

// Get the size of the file by its file descriptor
unsigned long get_size_by_fd(int fd)
{
	struct stat statbuf;
	if(fstat(fd, &statbuf) < 0) exit(-1);
	return statbuf.st_size;
}

std::string md5_hash(std::string filename)
{
	int file_descript;
	unsigned long file_size;
	char* file_buffer;
	unsigned char result[MD5_DIGEST_LENGTH];

	printf("using file:\t%s\n", filename.c_str());

	file_descript = open(filename.c_str(), O_RDONLY);
	if(file_descript < 0) return 0;

	file_size = get_size_by_fd(file_descript);
	printf("file size:\t%lu\n", file_size);

	file_buffer = (char *) mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
	MD5((unsigned char*) file_buffer, file_size, result);
	munmap(file_buffer, file_size);

	char md5string[2 * MD5_DIGEST_LENGTH + 1];

	memset(md5string, 0, 2 * MD5_DIGEST_LENGTH + 1);

	for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
		sprintf(&md5string[i*2], "%02x", (unsigned int)result[i]);

	std::string result_str (reinterpret_cast<char*>(md5string));

	syslog(LOG_NOTICE, "hash: %s", result_str.c_str());

	return result_str;
}

