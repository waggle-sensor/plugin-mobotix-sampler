##           ///          //                                  C++ Cross Platform
##          /////        ////
##         /// XXX     XXX ///            ///////////   /////////     ///   ///
##        ///    XXX XXX    ///         ///             ///    ///   ///  ///
##       ///       XXX       ///         /////////     ///      //  //////
##      ///      XXX XXX      ///               ///   ///    ///   ///  ///
##     ////    XXX     XXX    ////    ////////////  //////////    ///   ///
##    ////                     ////
##   ////  M  O  B  O  T  I  X  ////////////////////////////////////////////////
##  //// Security Vision Systems //////////////////////////////////////////////
##
##
######/
     ##
     ##  MOBOTIX EventStream Client SDK 
     ##
     ##  This software is licensed under the BSD license,
     ##  http:##www.opensource.org/licenses/bsd-license.php
     ##
     ##  Copyright (c) 2005 - 2016, MOBOTIX AG
     ##  All rights reserved.
     ##
     ##  Redistribution and use in source and binary forms, with or without
     ##  modification, are permitted provided that the following conditions are
     ##  met:
     ##
     ##  - Redistributions of source code must retain the above copyright
     ##    notice, this list of conditions and the following disclaimer.
     ##
     ##  - Redistributions in binary form must reproduce the above copyright
     ##    notice, this list of conditions and the following disclaimer in the
     ##    documentation and/or other materials provided with the distribution.
     ##
     ##  - Neither the name of MOBOTIX AG nor the names of its contributors may
     ##    be used to endorse or promote products derived from this software
     ##    without specific prior written permission.
     ##
     ##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     ##  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     ##  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
     ##  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     ##  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     ##  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
     ##  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
     ##  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
     ##  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
     ##  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
     ##  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     ##
######/

SDK_DIR := ../../

BUILD := build

BIN := live-stream

SRC_DIR := .

SOURCES := $(SRC_DIR)/main.cpp \
			$(SRC_DIR)/SinkVideo.cpp \
			$(SRC_DIR)/SinkAudio.cpp \
			$(SRC_DIR)/AuthorizationHandler.cpp \
			$(SRC_DIR)/NotificationListener.cpp

OBJS := $(addprefix $(BUILD)/, $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst sample/%,%,$(SOURCES)))))

include ../toolchain.cfg

INCLUDE_DIR := $(SDK_DIR)/include

LIB_DIR := $(SDK_DIR)/lib/$(OS)
LIB_EV_CLIENT := eventstreamclient

CXXFLAGS += -I$(INCLUDE_DIR) -std=c++11 -Wall -ggdb 

all: $(BIN)

clean:
	rm -rf $(BUILD) 

$(BIN): $(OBJS)
	@echo '##Building file: $<'
	$(CXX) $(CXXFLAGS) -o $(BUILD)/$(BIN) $(OBJS) -L$(LIB_DIR) -l$(LIB_EV_CLIENT)
	@echo '##Finished building: $<'

$(BUILD):
	@(if [ ! -d $(BUILD) ]; then \
		mkdir -p $(BUILD); \
	fi)

$(BUILD)/%.o: $(SRC_DIR)/%.cpp | $(BUILD)
	@echo '##Building file: $<'
	@echo '##Invoking: GCC C++ Compiler'
	$(CXX) $(CXXFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo '##Finished building: $<'
	@echo ' '	
