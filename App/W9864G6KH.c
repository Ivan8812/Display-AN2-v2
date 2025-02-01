#include "W9864G6KH.h"

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)


// Таймаут на выполнение команд
#define SDRAM_TIMEOUT  ((uint32_t)0xFFFF)


void sdram_init(SDRAM_HandleTypeDef *hsdram)
{
  FMC_SDRAM_CommandTypeDef command;

  //----------------------------------------------------------------------------
  // 1. Включаем тактирование SDRAM (Command Mode = CLOCK ENABLE)
  //----------------------------------------------------------------------------
  command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;  // или BANK2, если нужно
  command.AutoRefreshNumber      = 1;
  command.ModeRegisterDefinition = 0;

  HAL_SDRAM_SendCommand(hsdram, &command, SDRAM_TIMEOUT);

  // Согласно спецификации памяти, после включения такта нужно подождать не менее ~100-200 мкс.
  // HAL_Delay(1) даёт 1 мс, что достаточно с запасом.
  HAL_Delay(1);

  //----------------------------------------------------------------------------
  // 2. Команда PALL (Precharge All) - сброс всех банков в исходное состояние
  //----------------------------------------------------------------------------
  command.CommandMode            = FMC_SDRAM_CMD_PALL;
  command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  command.AutoRefreshNumber      = 1;
  command.ModeRegisterDefinition = 0;

  HAL_SDRAM_SendCommand(hsdram, &command, SDRAM_TIMEOUT);

  //----------------------------------------------------------------------------
  // 3. Команда Auto-Refresh (2 цикла подряд)
  //----------------------------------------------------------------------------
  command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  command.AutoRefreshNumber      = 2;  // Требуется минимум 2 цикла автообновления
  command.ModeRegisterDefinition = 0;

  HAL_SDRAM_SendCommand(hsdram, &command, SDRAM_TIMEOUT);

  //----------------------------------------------------------------------------
  // 4. Загрузка Mode Register
  //    Burst Length = 1, Sequential, CAS Latency = 3, Normal mode, Single Write
  //----------------------------------------------------------------------------
  command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  command.AutoRefreshNumber      = 1;
  command.ModeRegisterDefinition =
          SDRAM_MODEREG_BURST_LENGTH_1             // Burst Length = 1
        | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      // Sequential
        | SDRAM_MODEREG_CAS_LATENCY_3             // CAS = 3 (для 200 МГц)
        | SDRAM_MODEREG_OPERATING_MODE_STANDARD
        | SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;   // Single Write

  HAL_SDRAM_SendCommand(hsdram, &command, SDRAM_TIMEOUT);

  //----------------------------------------------------------------------------
  // 5. Программирование частоты автообновления (Refresh Rate)
  //
  // Формула расчёта из RM0433 (STM32H7) примерно такова:
  //   RefreshCount = ( (Период_обновления / Количество_строк) x Freq_SDRAM ) - 20
  //
  // Для W9864G6KH-5I обычно: 64ms на 4096 строк ⇒ 15.625 мкс между командами refresh.
  // При Freq_SDRAM = 200 МГц:
  //    15.625e-6 * 200e6 = ~3125 циклов
  // вычитаем ~20: получаем ~3105.
  // Можно округлить до 3100..3120. Часто берут 3122 или 3105 – зависит от запаса.
  //----------------------------------------------------------------------------
  HAL_SDRAM_ProgramRefreshRate(hsdram, 2344);

  // На этом инициализация SDRAM завершена.
}
