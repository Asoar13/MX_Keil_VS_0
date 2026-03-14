/*=========================电位器调整，按键选择，TM1637数码管显示，TB6612电机控制============================*/

#include "AdjustableFans/app_adjustablefans.h"
#include "Key/mid_key.h"
#include "ADC_DAC/mid_adc.h"
#include "Moter/mid_tb6612.h"
#include "Seg/mid_seg.h"
#include "LowPower/bsp_lowpower.h"
#include "GPIO/bsp_gpio.h"
#include "Time/bsp_tick.h"


#define _MAX_FREE_TIME_MS (60 * 1000)
#define _TWINKLE_TIME_MS  (500)
#define _RATE			  (0.5f)

// ADC继电器控制
MID_ADC_Handle_t h_adc_1 = {
		.h_adc = &hadc1,
};
// 按键选择
MID_Key_Handle_t h_key_1 = {
		.key_gpio_conf = {
			.gpiox = GPIOB,
			.gpio_pin_x = GPIO_PIN_8,
		}
};
// 切换按键模式的前提
BSP_GpioConf_t gpio_conf = {
	.gpiox = GPIOB,
	.gpio_pin_x = GPIO_PIN_8,
};
// 指示灯配置
BSP_GpioConf_t led_gpio_conf = {
	.gpiox = GPIOC,
	.gpio_pin_x = GPIO_PIN_13,
};
// 数码管显示
MID_Seg_Handele_t h_seg_1 = {
	.p_gpio_conf = {
		.gpio_sda =     GPIOB,
		.gpio_pin_sda = GPIO_PIN_13,
		.gpio_sck =     GPIOB,
		.gpio_pin_sck = GPIO_PIN_12,
	},
};
// tb6612电机控制
MID_TB6612_Hanle_t h_tb6612_1 = {
	.h_timx = &htim2,
	.TIM_CHANNEL_x = TIM_CHANNEL_1,

	.stby_gpio_conf = {
		.gpiox = GPIOA,
		.gpio_pin_x = GPIO_PIN_12,
	},
	.ctrl1_gpio_conf = {
		.gpiox = GPIOA,
		.gpio_pin_x = GPIO_PIN_10,
	},
	.ctrl2_gpio_conf = {
		.gpiox = GPIOA,
		.gpio_pin_x = GPIO_PIN_11,
	},
};

static uint8_t last_value;
static uint8_t now_value;
static uint8_t last_direct = 1;			// 默认正方向
static uint8_t now_direct;
static uint8_t direct_change_flag = 1;	// 默认识别转向
static uint8_t work_state = 0;			// 默认停止
static uint8_t speed;
static uint32_t last_twinkle_tick;
static uint32_t last_work_tick;

static void _segShowNum(uint8_t speed_num, uint8_t direct);
static void _setSpeed(uint8_t speed_duty, uint8_t direct_changed_flag);

typedef void (*PowerCtrl_Func) (void);
static void _enterSleep(void);
static void _enterStop(void);
static void _lowPowerMode(PowerCtrl_Func);


void APP_AdjustableFans_init(void)
{
	MID_ADC_init(&h_adc_1);
	MID_Key_init(&h_key_1);
	MID_Seg_init(&h_seg_1, 4);
	MID_TB6612_init(&h_tb6612_1);
}

void APP_AdjustableFans_process(void)
{
	uint8_t times = 0;
	uint32_t state = MID_Key_scan(&h_key_1, &times, GPIO_PIN_SET, 300, 100);

	/*-------------------------------------工作指示灯-----------------------------------------*/
	 if(BSP_Tick_getCurTick_32() - last_twinkle_tick > _TWINKLE_TIME_MS) {
		last_twinkle_tick = BSP_Tick_getCurTick_32();
		BSP_GPIO_togglePin(&led_gpio_conf);
	 }

	/*--------------------------------------按键选择------------------------------------------*/
	if(times == 1) { 	// 工作状态切换
		MID_TB6612_turnOffAndOn(&h_tb6612_1);
		work_state = work_state ? 0 : 1;
		last_work_tick = BSP_Tick_getCurTick_32();		// 更新记录开始的时间（自动睡眠）
	} else if (times == 2) {	// 待机模式
		_lowPowerMode(_enterSleep);
	}

	if (state > 2000) {	// 停机模式
		_lowPowerMode(_enterStop);
	}

	/*--------------------------------------自动睡眠------------------------------------------*/
	if(work_state == 1)	//  工作时一直更新记录开始的时间（自动睡眠）
	{
		last_work_tick = BSP_Tick_getCurTick_32();
	}

	// 非工作状态 且 超时 会触发睡眠
	if(BSP_Tick_getCurTick_32() - last_work_tick > _MAX_FREE_TIME_MS) {	// 达到设置时间自动进入睡眠模式
		last_work_tick = BSP_Tick_getCurTick_32();						// 更新记录开始的时间（自动睡眠）
		_lowPowerMode(_enterSleep);
	}

	/*--------------------------------------读修改值------------------------------------------*/
	MID_Seg_turnOff(&h_seg_1);
	now_value = MID_ADC_getValue(&h_adc_1, 200, 100);	// 0-99 正向，100-199 反向
	now_value = now_value * _RATE + last_value * (1 - _RATE);	// 加权滤波
	MID_Seg_turnOn(&h_seg_1);

	// 修改设置（值更新才设置新速度，方向改变才更新显示和方向）
	uint16_t d = (now_value > last_value) ? now_value - last_value : last_value - now_value;
	if(d > 1) {
		last_value = now_value;

		now_direct = (now_value < 100) ? 1 : 0;
		if(now_direct != last_direct) {
			last_direct = now_direct;
			direct_change_flag = 1;
		}

		now_value = (now_value == 200) ? 199 : now_value;				// 0-199可正常计算得出，200需要特殊处理
		speed = (now_direct) ? (99 - now_value) : (now_value % 100);	// 数字为 99-0-99 其中前半段正向，后半段反向

		_segShowNum(speed, now_direct);
		_setSpeed(speed, direct_change_flag);

		direct_change_flag = 0;
	}
}

void APP_AdjustaleFans_ADC_DMA_IRQ()
{
	MID_ADC_remindUpdate_IRQ(&h_adc_1);
}

static void _segShowNum(uint8_t speed_num, uint8_t direct)
{
	// 后两位是数字
	MID_Seg_setPosAndNum(&h_seg_1, 2, speed_num, 2, 0);

	// 前两位表示方向
	if(direct_change_flag || !speed) {
		if(now_direct == 0) {
			uint8_t seg_1[] = {0x4F, 0x4F};
			MID_Seg_setSeg(&h_seg_1, 0, seg_1, 2);
		} else if(now_direct == 1) {
			uint8_t seg_0[] = {0x79, 0x79};
			MID_Seg_setSeg(&h_seg_1, 0, seg_0, 2);
		}
	}
}

static void _setSpeed(uint8_t speed_duty, uint8_t direct_changed_flag)
{
	MID_TB6612_setSpeedDuty(&h_tb6612_1, speed_duty);
	if(direct_changed_flag)
		MID_TB6612_rotateInReverse(&h_tb6612_1);
}

// 睡眠模式仅关闭逻辑计算和检测
static void _enterSleep()
{
	// 显示“SLEP”字样
	uint8_t seg_sleep[] = {0x6D, 0x38, 0x79, 0x73};
	MID_Seg_setSeg(&h_seg_1, 0, seg_sleep, 4);

	// 关闭正常工作指示灯
	BSP_GPIO_setPin(&led_gpio_conf);

	// 进入睡眠模式
	BSP_LowPower_sleep();

	// 更新显示
	direct_change_flag = 1; 
	_segShowNum(speed, now_direct);
}

// 停止模式下关闭一切显示和控制
static void _enterStop()
{
	MID_Seg_turnOff(&h_seg_1);
	BSP_LowPower_stop();
	MID_Seg_turnOn(&h_seg_1);
}

static void _lowPowerMode(PowerCtrl_Func _enterLowPower)
{
	// 进入中断模式
	BSP_GPIO_setMode(&gpio_conf, BSP_PIN_8_RAISING_EXIT_GROUP2_2_2);

	// 关闭systick时钟
	BSP_Tick_turnOff();

	// 进入低功耗
	_enterLowPower();

	// 关闭指定引脚中断（sleep结束就还原）
	BSP_GPIO_disableEXIT(&gpio_conf, BSP_PIN_8_RAISING_EXIT_GROUP2_2_2);
	BSP_GPIO_setMode(&gpio_conf, BSP_PIN_IN_PULLDOWN);

	// 开启systick时钟
	BSP_Tick_turnOn();
}


