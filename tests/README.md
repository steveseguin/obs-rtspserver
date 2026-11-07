# AAC SDP regression test

The test compiles the actual AAC source without OBS, Qt, network access, or media
devices. From the repository root on Linux with a C++14 compiler:

```sh
c++ -std=c++14 -Wall -Wextra -Werror -Irtsp-server \
  tests/aac_sdp_test.cpp rtsp-server/xop/AACSource.cpp -o /tmp/aac_sdp_test
/tmp/aac_sdp_test
```

It checks the complete SDP attributes for stereo and multichannel AAC-LC,
including hexadecimal configuration values containing `a` or `b`. This is a
source-level serialization test, not a full OBS plugin or RTSP playback test.
