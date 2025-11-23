#include "Delay.h"

/**
  * @brief  初始化DWT(Data Watchpoint and Trace)单元
  * @param  无
  * @retval 0-成功; 1-失败(DWT不可用)
  */
uint8_t DWT_Init(void)
{
    // 启用DWT跟踪
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    // 重置周期计数器
    DWT->CYCCNT = 0;
    
    // 启用周期计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    // 检查DWT是否可用
    if(DWT->CYCCNT)
    {
        return 0;  // 初始化成功
    }
    else
    {
        return 1;  // DWT不可用
    }
}

/**
  * @brief  纳秒级延时(近似)
  * @param  ns 延时的纳秒数
  * @retval 无
  * @note   在400MHz下，1个时钟周期=2.5ns，这是近似延时
  *         最小延时精度约2.5ns
  */
void Delay_ns(uint32_t ns)
{
    uint32_t start = DWT->CYCCNT;
    // 400MHz → 0.4 cycles/ns
    uint32_t cycles = (ns * 2) / 5;  // 整数近似计算，避免浮点
    if(cycles < 1) cycles = 1;       // 至少延时1个周期(2.5ns)
    while ((DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  微秒级精确延时
  * @param  us 延时的微秒数
  * @retval 无
  * @note   在400MHz下，1μs=400个周期
  */
void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * 400;  // 400MHz → 400 cycles/μs
    // 处理32位计数器溢出情况
    while ((uint32_t)(DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  毫秒级精确延时
  * @param  ms 延时的毫秒数
  * @retval 无
  * @note   在400MHz下，1ms=400,000个周期
  */
void Delay_ms(uint32_t ms)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = ms * 400000;  // 400MHz → 400,000 cycles/ms
    while ((uint32_t)(DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  秒级延时
  * @param  s 延时的秒数
  * @retval 无
  * @note   使用毫秒延时实现，避免计数器溢出
  */
void Delay_s(uint32_t s)
{
    while(s--)
    {
        Delay_ms(1000);  // 调用毫秒延时1000次
    }
}