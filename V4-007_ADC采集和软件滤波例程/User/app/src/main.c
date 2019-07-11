/*
*********************************************************************************************************
*
*	ģ������ : ���������
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ����ͨ�����ӣ���PC�������ն�������н���
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2015-08-30 armfly  �׷�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"				/* �ײ�Ӳ������ */

/* ���������������̷������� */
#define EXAMPLE_NAME	"V4-006_ADC�ɼ�����"
#define EXAMPLE_DATE	"2015-08-30"
#define DEMO_VER		"1.0"

static void PrintfLogo(void);
static void PrintfHelp(void);
static uint16_t GetADC(void);
static void ADC_Configuration(void);

#define SAMP_COUNT	20		/* ������������ʾ200ms�ڵĲ���������ƽ��ֵ */

uint16_t g_usAdcValue;	/* ADC ����ֵ��ƽ��ֵ */

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;	

	/*
		ST�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� system_stm32f4xx.c �ļ�����Ҫ������
	����CPUϵͳ��ʱ�ӣ��ڲ�Flash����ʱ������FSMC�����ⲿSRAM
	*/
	bsp_Init();		/* Ӳ����ʼ�� */
	
	PrintfLogo();	/* ��ӡ�������ƺͰ汾����Ϣ */
	PrintfHelp();	/* ��ӡ������ʾ */

	ADC_Configuration();	/* ����PC4 ΪADC1_IN14 */

	bsp_StartAutoTimer(1, 300);	/* ��ʱ��0���� 300���� */

	/* ����������ѭ���� */
	while (1)
	{
		bsp_Idle();		/* CPU����ʱִ�еĺ������� bsp.c */

		if (bsp_CheckTimer(1))	/* ��ʱ�� */
		{
			uint16_t adc;
			
			adc = GetADC();

			/* ע�⣺ ĩβֻ�� \r�س�, û��\n���У�����ʹPC�����ն˽����ȶ���1����ʾ */
			{
				/* �����ն˽����ϻ���ʾһ��������ת���ַ�
				����������ܣ���Ϊ�˱�����������ļ�����ΪADC����ֵ���ȶ�
				*/
				static uint8_t pos = 0;

				if (pos == 0)
					printf("|");
				else if (pos == 1)
					printf("/");
				else if (pos == 2)
					printf("-");
				else if (pos == 3)
					printf("\\");		/* ע�⣺��������ַ���Ҫת�� */

				if (++pos >= 4)
				{
					pos = 0;
				}

				printf(" PC4����ADC����ֵ = %5d , ��ѹ = %4dmV\r",
					adc, ((uint32_t)adc * 2500) / 4095);

			}
		}

		/* �������¼� */
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode > 0)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:		
					break;		
				
				case KEY_DOWN_K2:
					break;	
					
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ADC_Configuration
*	����˵��: ����ADC, PC4��ΪADCͨ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	__IO uint16_t ADCConvertedValue;

    /* ʹ�� ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

	/* ����PC4Ϊģ������(ADC Channel14) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ����ADC1, ����DMA, ��������� */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ����ADC1 ����ͨ��14 channel14 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);

	/* ʹ��ADC1 DMA���� */
	ADC_DMACmd(ADC1, ENABLE);

	/* ʹ�� ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* ʹ��ADC1 ��λУ׼�Ĵ��� */
	ADC_ResetCalibration(ADC1);
	/* ���ADC1�ĸ�λ�Ĵ��� */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ����ADC1У׼ */
	ADC_StartCalibration(ADC1);
	/* ���У׼�Ƿ���� */
	while(ADC_GetCalibrationStatus(ADC1));

	/* �������ADCת�� */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: AdcPro
*	����˵��: ADC������������1ms systick �жϽ��е���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AdcPro(void)
{
	static uint16_t buf[SAMP_COUNT];
	static uint8_t write;
	uint32_t sum;
	uint8_t i;

	buf[write] = ADC_GetConversionValue(ADC1);
	if (++write >= SAMP_COUNT)
	{
		write = 0;
	}

	/* ������δ��������ƽ��ֵ�ķ��������˲�
		Ҳ���Ը����£�ѡ��ȥ����������2��ֵ��ʹ���ݸ��Ӿ�ȷ
	*/
	sum = 0;
	for (i = 0; i < SAMP_COUNT; i++)
	{
		sum += buf[i];
	}
	g_usAdcValue = sum / SAMP_COUNT;	/* ADC����ֵ�����ɴβ���ֵƽ�� */

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	/* ��������´�ADCת�� */
}

/*
*********************************************************************************************************
*	�� �� ��: GetADC
*	����˵��: ��ȡADC������ƽ��ֵ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static uint16_t GetADC(void)
{
	uint16_t ret;

	/* ��Ϊ	g_AdcValue ������systick�ж��и�д��Ϊ�˱��������������ʱ���жϳ�����ҵ������ݴ��������Ҫ
	�ر��жϽ��б��� */

	/* �����ٽ����������ر��ж� */
	__set_PRIMASK(1);  /* ���ж� */

	ret = g_usAdcValue;

	__set_PRIMASK(0);  /* ���ж� */

	return ret;
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfHelp
*	����˵��: ��ӡ������ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
	printf("����ڿ������ϵľ��ܿɵ����裬�۲�ADC����ֵ�ı仯\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾���궨���� stm32f4xx.h �ļ� */
	printf("* �̼���汾 : %d.%d.%d\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,__STM32F10X_STDPERIPH_VERSION_SUB2);

	/* ��ӡ CMSIS �汾. �궨���� core_cm4.h �ļ� */
	printf("* CMSIS�汾  : %X.%02X\r\n", __CM3_CMSIS_VERSION_MAIN, __CM3_CMSIS_VERSION_SUB);

	printf("* \n\r");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
