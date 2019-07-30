实验器材:
	潘多拉STM32L4 IOT开发板 
	
实验目的:
	本实验为新建工程实验，仅供大家新建工程时参考。
	新建工程详细步骤，请看《STM32L4开发指南-HAL版本》第3.3节。
	
硬件资源:
	1,潘多拉STM32L4开发板
	
实验现象:
	本实验下载后，RGB红色灯闪烁。 

工程文件来源：

①HAL库文件                                                                 ->   \STM32Cube_FW_L4_V1.13.0\Drivers\STM32L4xx_HAL_Driver
②startup_stm32l475xx.s                                                     ->   \STM32Cube_FW_L4_V1.13.0\Drivers\CMSIS\Device\ST\STM32L4xx\Source\Templates\arm
③cmsis_armcc.h，core_cm4.h，core_cmFunc.h，core_cmInstr.h ，core_cmSimd.h  ->   \STM32Cube_FW_L4_V1.13.0\Drivers\CMSIS\Include

④stm32l4xx.h，system_stm32l4xx.h,stm32l4xx_hal_msp.c和stm32l475xx.         ->   \STM32Cube_FW_L4_V1.13.0\Drivers\CMSIS\Device\ST\STM32L4xx\Include

⑤stm32l4xx_it.h，stm32l4xx_hal_conf.h 和main.h                             ->   \STM32Cube_FW_L4_V1.13.0\Projects\B-L475E-IOT01A\Templates\Inc
⑥system_stm32l4xx.c，stm32l4xx_it.c和main.c          			    ->   \STM32Cube_FW_L4_V1.13.0\Projects\B-L475E-IOT01A\Templates\Scr

SYSTEM文件夹，任何一个HAL库实验工程中有
	
注意事项:
	无.


					正点原子@ALIENTEK
					2016-6-24
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://openedv.taobao.com
					http://eboard.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com