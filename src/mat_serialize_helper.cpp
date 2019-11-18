/*
 * Copyright 2019 SU Technology Ltd. All rights reserved.
 *
 * MIT License
 *
 * Copyright (c) 2019 SU Technology
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
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include "mat_serialize_helper.h"

QByteArray mat2ByteArray(const cv::Mat &image)
{
	QByteArray byteArray;
	QDataStream stream( &byteArray, QIODevice::WriteOnly );
	stream << image.type();
	stream << image.rows;
	stream << image.cols;
	const size_t data_size = image.cols * image.rows * image.elemSize();
	QByteArray data = QByteArray::fromRawData( (const char*)image.ptr(), data_size );
	stream << data;
	return byteArray;
}

cv::Mat byteArray2Mat(const QByteArray & byteArray)
{
	QDataStream stream(byteArray);
	int matType, rows, cols;
	QByteArray data;
	stream >> matType;
	stream >> rows;
	stream >> cols;
	stream >> data;
	cv::Mat mat( rows, cols, matType, (void*)data.data() );
	return mat.clone();
}
