TARGET=my_thread
obj-m+=${TARGET}.o

## Orange PI compiling
ENV_IP=10.42.0.35
ENV_PORT=22
ENV_USER=ovdiyuk
BUILD_KERNEL+=/lib/modules/4.14.84-sunxi/build

## Options for croos-compiling
# export ARCH = arm
# export CROSS_COMPILE ?= arm-linux-gnueabihf-

## To compile at QEMU
# ENV_IP=127.0.0.1
# ENV_PORT=8022
# ENV_USER=user
# BUILD_KERNEL+=${HOME}/env/build/linux-stable
# BUILD_ROOTFS+=${HOME}/env/build/buildroot

all:
	make -C ${BUILD_KERNEL} M=$(shell pwd) modules
clean:
	make -C ${BUILD_KERNEL} M=$(shell pwd) clean
clean-env:
	ssh ${ENV_USER}@${ENV_IP} -p ${ENV_PORT} "rm -f /home/${ENV_USER}/${TARGET}.*"
send-env:
	scp -v -P ${ENV_PORT} ${TARGET}.* ${ENV_USER}@${ENV_IP}:/home/${ENV_USER}
	scp -v -P ${ENV_PORT} Makefile ${ENV_USER}@${ENV_IP}:/home/${ENV_USER}
all-env:
	ssh ${ENV_USER}@${ENV_IP} -p ${ENV_PORT} "make -C ${BUILD_KERNEL} M=$(shell pwd) modules"
