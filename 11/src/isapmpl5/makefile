

TARGET = test

SOURCES = nullimplfrvt11.cpp
OBJECTS = $(SOURCES:%.c=%.o)

LIB_OPENCV = -L./opencv/lib -lopencv_calib3d -lopencv_highgui -lopencv_objdetect -lopencv_imgproc -lopencv_core -lIlmImf -llibjasper -llibjpeg -llibpng -llibtiff

LIB_TF_SHARE = -L/usr/local/lib -ltensorflow_cc -ltensorflow_framework -lprotobuf
LIBS = -L./ -l:libkenxnet.so $(LIB_OPENCV) $(LIB_TF_SHARE) -lpthread -lz -ldl -lstdc++

INCLUDES = -I/usr/include \
-I./opencv/include \
-I../include \
-I../../../common/src/include


CFLAGS = -g -fexceptions -Wno-deprecated-declarations -Wno-unused-result -Wno-format-contains-nul -Wno-conversion-null -Wno-pointer-arith -Wno-format

CC = g++ -std=c++11 -Wl,-rpath=./

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(INCLUDES) $(CFLAGS) $(LIBS)

$(TARGET).so: $(OBJECTS)
	$(CC) -o $@ $^ $(INCLUDES) $(CFLAGS) $(LIBS) -fPIC -shared
#Cleanup
.PHONY: clean

clean:
	rm -f *.o *~ core *~
