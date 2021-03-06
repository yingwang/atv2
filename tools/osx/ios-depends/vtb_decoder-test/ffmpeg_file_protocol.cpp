// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include <unistd.h>
#include <fcntl.h>

#include "ffmpeg_common.h"
#include "ffmpeg_file_protocol.h"

namespace {

int GetHandle(URLContext *h) {
  return static_cast<int>(reinterpret_cast<intptr_t>(h->priv_data));
}

// FFmpeg protocol interface.
int OpenContext(URLContext* h, const char* filename, int flags) {
  int access = O_RDONLY;
  if (flags & URL_RDWR) {
    access = O_CREAT | O_TRUNC | O_RDWR;
  } else if (flags & URL_WRONLY) {
    access = O_CREAT | O_TRUNC | O_WRONLY;
  }
#ifdef O_BINARY
  access |= O_BINARY;
#endif
  int f = open(filename, access, 0666);
  if (f == -1)
    return AVERROR(ENOENT);
  h->priv_data = reinterpret_cast<void*>(static_cast<intptr_t>(f));
  h->is_streamed = false;
  return 0;
}

int ReadContext(URLContext* h, unsigned char* buf, int size) {
  return read(GetHandle(h), buf, size);
}

int WriteContext(URLContext* h, const unsigned char* buf, int size) {
  return write(GetHandle(h), buf, size);
}

offset_t SeekContext(URLContext* h, offset_t offset, int whence) {
  return lseek(GetHandle(h), offset, whence);
}

int CloseContext(URLContext* h) {
  return close(GetHandle(h));
}

}  // namespace

URLProtocol kFFmpegFileProtocol = {
  "file",
  &OpenContext,
  &ReadContext,
  &WriteContext,
  &SeekContext,
  &CloseContext,
  NULL,  // *next
  NULL,  // url_read_pause
  NULL,  // url_read_seek
  &GetHandle
};
