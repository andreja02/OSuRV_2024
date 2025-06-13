#!/bin/bash

exit 0


./waf configure

./waf build && ./build/dump_log

