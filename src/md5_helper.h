/*
 * md5_helper.h
 *
 *  Created on: 13 Sep 2019
 *      Author: root
 */

#ifndef SRC_MD5_HELPER_H_
#define SRC_MD5_HELPER_H_

#include <openssl/md5.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string>
#include <syslog.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

// Get the size of the file by its file descriptor
unsigned long get_size_by_fd(int fd);
std::string md5_hash(std::string filename);

#endif /* SRC_MD5_HELPER_H_ */
