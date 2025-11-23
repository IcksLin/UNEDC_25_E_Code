#include "Delay.h"

/**
  * @brief  初始化DWT(Data Watchpoint and Trace)周期计数器
  * @retval 1=成功, 0=失败
  */
uint8_t DWT_Init(void)
{
    /* 解锁DWT */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    /* 重置周期计数器 */
    DWT->CYCCNT = 0;
    
    /* 启用周期计数器 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    /* 检查是否支持DWT */
    return (DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) ? 0 : 1;
}

/**
  * @brief  纳秒级延时(近似)
  * @param  ns 延时的纳秒数
  * @note   400MHz下1个时钟周期=2.5ns
  *         最小延时精度=3ns（1周期）
  */
void Delay_ns(uint32_t ns)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = (ns + 2) / 3;  // 向上取整，400MHz时3ns/cycle
    while((DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  微秒级高精度延时
  * @param  us 延时的微秒数
  * @note   400MHz下1μs=400周期
  */
void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * 400;  // 400 cycles/μs
    /* 处理32位计数器溢出（约10.7秒溢出一次）*/
    if(cycles > (0xFFFFFFFF - start)) {
        while(DWT->CYCCNT > start);  // 等待溢出
        start = 0;
        cycles -= (0xFFFFFFFF - start);
    }
    while((DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  毫秒级延时
  * @param  ms 延时的毫秒数
  */
void Delay_ms(uint32_t ms)
{
    while(ms--) {
        Delay_us(1000);  // 400,000 cycles/ms
    }
}

/**
  * @brief  秒级延时
  * @param  s 延时的秒数
  */
void Delay_s(uint32_t s)
{
    while(s--) {
        Delay_ms(1000);  // 400,000,000 cycles/s
    }
}