# MPU6050 Practice. Kernel object module development

## Homework

1. Create simplest driver (kernel object module) using compiled kernel (by Armbian or without it)  
	and cross-compiler.
2. Fix errors in driver mpu6050 from repository.
3. Сompile it using cross-compiler.
4. Deploy and run it. Check output using dmesg.
5. Check modified DTS.
6. Check driver’s output by reading value from sysfs


### Lesson 11 - Interrupts handling

1. Connect mpu6050 interrupt line to OrangePi gpio pin.
2. Setup selected gpio as interrupt input.
3. Use tasklet and workqueue for deferred processing of gpio interrupts.  
   (Timer event also may be used as trigger for workqueue scheduling)
4. Move mpu6050 values reading to workqueue.
5. Implement additional processing in deffered stages of interrupts handling.  
   (e.g. filtering)
