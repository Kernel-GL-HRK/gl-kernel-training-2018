
MODULE_PROCFS=mymodule_procfs
MODULE_SYSFS=mymodule_sysfs

obj-m+=${MODULE_PROCFS}.o 
obj-m+=${MODULE_SYSFS}.o

ENV_IP+=127.0.0.1
ENV_PORT=8022
ENV_USER=user
BUILD_KERNEL+=${HOME}/env/build/kernel
BUILD_ROOTFS+=${HOME}/env/build/rootfs

all:
	make -C ${BUILD_KERNEL} M=$(shell pwd) modules
clean:
	make -C ${BUILD_KERNEL} M=$(shell pwd) clean
clean-env: 
	ssh ${ENV_USER}@${ENV_IP} -p ${ENV_PORT} "rm /home/${ENV_USER}/${MODULE_PROCFS}.ko"
	ssh ${ENV_USER}@${ENV_IP} -p ${ENV_PORT} "rm /home/${ENV_USER}/${MODULE_SYSFS}.ko"
send-env: 
	scp -v -P ${ENV_PORT} ${MODULE_PROCFS}.ko ${ENV_USER}@${ENV_IP}:/home/${ENV_USER}
	scp -v -P ${ENV_PORT} ${MODULE_SYSFS}.ko ${ENV_USER}@${ENV_IP}:/home/${ENV_USER}
test-env: all clean-env send-env clean