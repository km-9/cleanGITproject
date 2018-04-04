#/*
# * Copyright (C) 2014  RoboPeak
# *
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *
# */
#
HOME_TREE := ../../

MODULE_NAME := $(notdir $(CURDIR))
SOURCE=wallFollowerTest1.cpp
PROGRAM=wallFollowerTest1
INCLUDESPATH=/home/pi/Downloads/lidar_sdk/sdk/sdk/include
INCLUDES=rplidar.h rplidar_driver.h rplidar_protocol.h rptypes.h rplidar_cmd.h wallFollower.h point.hpp interpolator.hpp lidar_support.hpp templateMatcher.h
LIBRARIESPATH=/home/pi/Downloads/lidar_sdk/sdk/output/Linux/Release
LIBRARIES= -lrplidar_sdk
CC=g++
CXXFLAGS=-std=c++14

include $(HOME_TREE)/mak_def.inc

CXXSRC += main.cpp PCA9685.cpp wallFollower.cpp interpolator.cpp lidar_support.cpp point.cpp templateMatcher.cpp
C_INCLUDES += -I$(CURDIR)/../../sdk/include

EXTRA_OBJ :=
LD_LIBS += -lstdc++ -lpthread -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lopencv_video -lopencv_imgproc -lopencv_highgui -lwiringPi

all: build_app

include $(HOME_TREE)/mak_common.inc

clean: clean_app
