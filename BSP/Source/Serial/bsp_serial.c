//#include <Serial/bsp_serial.h>
//
//
//#ifdef HAL_UART_MODULE_ENABLED
//	HAL_StatusTypeDef BSP_Serial_transmitByte_IT(void *huartx, const uint8_t *p_data, uint16_t len)
//	{
//		return HAL_UART_Transmit_IT((UART_HandleTypeDef *)huartx, p_data, len);
//	}
//
//	HAL_StatusTypeDef BSP_Serial_transmitByte(void *huartx, const uint8_t *p_data, uint16_t len)
//	{
//		return HAL_UART_Transmit((UART_HandleTypeDef *)huartx, p_data, len, 100);
//	}
//
//	HAL_StatusTypeDef BSP_Serial_receiveByte_IT(void *huartx, uint8_t *p_byte)
//	{
//		return HAL_UART_Receive_IT((UART_HandleTypeDef *)huartx, p_byte, 1);
//	}
//
//	HAL_StatusTypeDef BSP_Serial_receiveByte(void *huartx, uint8_t *p_byte)
//	{
//		return HAL_UART_Receive((UART_HandleTypeDef *)huartx, p_byte, 1, 100);
//	}
//
//#else
//	HAL_StatusTypeDef BSP_Serial_transmitByte_IT(void *huartx, const uint8_t *p_byte)
//	{
//		// 等待软件UART
//	}
//
//	HAL_StatusTypeDef BSP_Serial_receiveByte_IT(void *huartx, uint8_t *p_byte)
//	{
//		// 等待软件UART
//	}
//
//	HAL_StatusTypeDef BSP_Serial_transmitByte(void *huartx, const uint8_t *p_byte)
//	{
//		// 等待软件UART
//	}
//
//	HAL_StatusTypeDef BSP_Serial_receiveByte(void *huartx, uint8_t *p_byte)
//	{
//		// 等待软件UART
//	}
//
//#endif /* HAL_UART_MODULE_ENABLED */
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
