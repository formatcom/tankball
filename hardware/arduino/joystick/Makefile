#REF: https://github.com/sudar/Arduino-Makefile

include $(ARDMK_DIR)/Arduino.mk

ifeq ($(SENSOR), mpu6050)
	CXXFLAGS +=-DSENSOR_MPU6050
else
	CXXFLAGS +=-DSENSOR_ULTRASONIC
endif

# cp env.bk env
# --- MODIFIED ENV
# source ./env
# make show_boards
# --- CHOICE BOARD
# make -e BOARD_TAG=leonardo
# make -e SENSOR=mpu6050
# sudo -E make -e BOARD_TAG=leonardo do_upload

# REF: https://arduino.stackexchange.com/questions/25346/upload-code-on-arduino-leonardo-using-command-line
# REF: https://github.com/vanbwodonk/leonardoUploader/blob/master/linux/leonardoUploader.cpp
ifeq ($(BOARD_TAG), leonardo)
do_upload:
	stty -F $(call get_monitor_port) ispeed 1200 ospeed 1200 # reset arduino
	sleep 1 # wait reset
	$(AVRDUDE) $(AVRDUDE_COM_OPTS) $(AVRDUDE_ARD_OPTS) $(AVRDUDE_UPLOAD_HEX) # upload
endif
