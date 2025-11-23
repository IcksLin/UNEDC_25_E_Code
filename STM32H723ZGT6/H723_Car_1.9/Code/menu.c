#include "Menu.h"
// 按键变量
KeyInfo keys[KEY_NUM];
const GPIO_TypeDef *KEY_PORT[KEY_NUM] = {KEY0_GPIO_Port, KEY1_GPIO_Port, KEY2_GPIO_Port, KEY3_GPIO_Port};
const uint16_t KEY_PIN[KEY_NUM] = {KEY0_Pin, KEY1_Pin, KEY2_Pin, KEY3_Pin};
// flag to indicate if in mode interaction,this flag need to be set by menu callback function when return to menu
static uint8_t mode_inter_flag = 0;

uint8_t pid_control_car_flag = 0; // 车pid控制标志位

// 按键扫描函数，初始化代码由HAL库生成
// 该函数使用状态机，仅实现短按效果，需要拓展自行修改代码
// 按键的检测结果（即按键是否被短按）存储在 keys[i].short_press_flag 变量中,该标志置位需要手动清零
void scan_keys(void)
{
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        uint8_t pin_level = (HAL_GPIO_ReadPin((GPIO_TypeDef *)KEY_PORT[i], KEY_PIN[i]) == GPIO_PIN_RESET) ? 1 : 0;

        switch (keys[i].state)
        {
        case KEY_RELEASED:
            if (pin_level)
            {
                keys[i].state = KEY_DEBOUNCE;
                keys[i].counter = 0;
            }
            break;

        case KEY_DEBOUNCE:
            if (pin_level)
            {
                if (++keys[i].counter >= KEY_DEBOUNCE_TIME)
                {
                    keys[i].state = KEY_PRESSED;
                    keys[i].counter = 0; // 重置计数器用于长按计时
                }
            }
            else
            {
                keys[i].state = KEY_RELEASED;
            }
            break;

        case KEY_PRESSED:
            if (!pin_level)
            {
                keys[i].short_press_flag = 1; // 标记短按
                keys[i].state = KEY_RELEASED;
            }
            else if (++keys[i].counter >= KEY_LONG_PRESS_TIME)
            {
                keys[i].state = KEY_LONG_PRESS; // 进入长按状态
            }
            break;

        case KEY_LONG_PRESS:
            if (!pin_level)
            {
                keys[i].long_press_flag = 1; // 松开时才标记长按
                keys[i].state = KEY_RELEASED;
            }
            break;

        default:
            keys[i].state = KEY_RELEASED;
            break;
        }
    }
}

/**
 * @brief 将按键标志位转换为菜单动作
 * @return MenuAction 枚举值（包含WAITING表示无操作）
 */
MenuAction get_menu_action(void)
{
    static const MenuAction key_mapping[KEY_NUM] = {
        [KEY_UP_IDX] = MENU_UP,
        [KEY_DOWN_IDX] = MENU_DOWN,
        [KEY_OK_IDX] = MENU_OK,
        [KEY_BACK_IDX] = MENU_BACK};

    // 检测长按（松开后触发）
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (keys[i].long_press_flag)
        {
            MenuAction action = (MenuAction)(key_mapping[i] + 5); // 长按动作=原值+5
            keys[i].long_press_flag = 0;                          // 菜单按键映射情况下该标志位

            return action;
        }
    }

    // 检测短按（自动清除标志）
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (keys[i].short_press_flag)
        {
            keys[i].short_press_flag = 0;
            return key_mapping[i];
        }
    }

    return WAITING;
}

// Menu option items
// 需要实现菜单选项功能在这里替换回调函数即可
// Mode menu items
Menu IMU_angle = {"IMU_Angle", 9, option_func, NULL, NULL, NULL};
Menu gray_calibration = {"Gray_Calibration", 8, option_func, NULL, NULL, &IMU_angle};
Menu tripod_pid = {"Tripod_PID", 7, option_func, NULL, NULL, NULL};
Menu car_pid = {"Car_PID", 6, option_func, NULL, NULL, &tripod_pid};
Menu Pid_Set = {"Set_PID", 5, option_func, NULL, &car_pid, &gray_calibration};

Menu mode5 = {"Mode5", 4, option_func, NULL, &Pid_Set, NULL};
Menu mode4 = {"Mode4", 3, option_func, NULL, NULL, &mode5};
Menu mode3 = {"Mode3", 2, option_func, NULL, NULL, &mode4};
Menu mode2 = {"Mode2", 1, option_func, NULL, NULL, &mode3};
Menu mode1 = {"Mode1", 0, option_func, NULL, NULL, &mode2};
// Main menu
Menu main_menu = {"Main Menu", -1, NULL, NULL, &mode1, NULL};
Menu *current_menu = &main_menu;

// 初始化菜单的parent关系
// 这里设置每个菜单项的parent指针，便于在菜单系统中进行导航
void init_menu_parents(void)
{
    // 模式的parent
    mode1.parent = &main_menu;
    mode2.parent = &main_menu;
    mode3.parent = &main_menu;
    mode4.parent = &main_menu;
    mode5.parent = &main_menu;

    Pid_Set.parent = &mode5;
    car_pid.parent = &Pid_Set;
    tripod_pid.parent = &Pid_Set;
    gray_calibration.parent = &mode5;
    IMU_angle.parent = &mode5;
}

// 菜单切换函数，接受值为0-3的整数，表示确定，上下，返回
//  获取同级菜单的上一个（需要从parent的child链表遍历）
Menu *menu_get_prev_sibling(Menu *current)
{
    if (!current || !current->parent || current->parent->child == current)
        return NULL;
    Menu *p = current->parent->child;
    while (p && p->sibling != current)
    {
        p = p->sibling;
    }
    return p;
}

// 主遍历函数，传入操作变量，返回菜单地址
Menu *menu_navigate(Menu *current, MenuAction action)
{
    if (!current)
        return NULL;

    switch (action)
    {
    case MENU_UP:
    {
        Menu *prev = menu_get_prev_sibling(current);
        if (prev)
            return prev;
        break;
    }
    case MENU_DOWN:
        if (current->sibling)
            return current->sibling;
        break;
    case MENU_OK:
        if (current->child)
        {
            // if (current->child->func) current->child->func();
            return current->child;
        }
        else
        {
            if (current->func)
                current->func();
            return current;
        }
        break;
    case MENU_BACK:
        if (current->parent)
            return current->parent;
        break;
    default:
        return current;
    }

    // 显示当前菜单项
    // OLED_ShowString(0*16,0*16,current->i, OLED_8X16);

    // 若无可跳转项，返回自身
    return current;
}

// 示例回调函数(实例)
void option_func(void)
{
    if (mode_inter_flag == 0)
        mode_inter_flag = 1;
    return;
}

void init_menu(void)
{
    // 初始化菜单结构体
    init_menu_parents();
    current_menu = &main_menu;
    OLED_Clear();
    OLED_Update();
}

void OLED_DrawMenu(Menu *selected_menu)
{
    // 1. 找到同级菜单链表头
    Menu *head = selected_menu;
    while (head->parent && head->parent->child && head->parent->child != head)
    {
        head = head->parent->child;
    }

    // 2. 遍历同级链表，统计总数和选中项序号
    int total = 0;
    int selected_index = -1;
    Menu *iter = head;
    while (iter)
    {
        if (iter == selected_menu)
            selected_index = total;
        total++;
        iter = iter->sibling;
    }

    // 3. 计算分页起点
    uint8_t page_start = 0;
    if (selected_index >= MENU_MAX_ROW)
        page_start = selected_index - (MENU_MAX_ROW - 1);

    // 4. 定位到page_start
    iter = head;
    for (int i = 0; i < page_start && iter; ++i)
        iter = iter->sibling;

    // 5. 清屏并显示4项
    for (int row = 0; row < MENU_MAX_ROW && iter; row++)
    {
        int y = row * FONT_H;
        if ((page_start + row) == selected_index)
        {
            OLED_DrawRectangle(0, y, 64, FONT_H, OLED_UNFILLED); // 高亮选中项
        }
        OLED_ShowString(2, y + 2, iter->name, OLED_8X16);
        iter = iter->sibling;
    }
}

/******************************menu callback function(start)******************************/
// id:0，激光锁点，无巡视
void menu_mode_0(uint8_t cl_action)
{
    static int start_flag = 0;
    static int start_time = 0;
    static int current_time = 0;
    current_time = uwTick; // 获取当前时间戳
    pid_control_car_flag = 1;

    if (start_flag == 1 && (current_time - start_time) >= 1600)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); // Set the start pin to high
    }
    if (start_flag == 1) //
    {
        if (rect.x != 999 && rect.y != 999)
        {
            move_distance.x = rect.y - target_point.y;
            move_distance.y = rect.x - target_point.x;
        }
        else
        {
            move_distance.x = 0;
            move_distance.y = 0;
        }
    }

    switch (cl_action)
    {
    case 0:
        start_flag = 1; // set start flag
        start_time = uwTick;

        break;
    case 1:

        break;
    case 2:

        break;
    case 3:
        mode_inter_flag = 0; // return to menu
        pid_control_car_flag = 0;
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
        start_flag = 0; // reset start flag
        start_time = 0; // reset start time
        break;

    default:
        break;
    }

    /**************function_task(start)*****************/
    // /*************get Rect info by USART*************/

    /**************function_task(end)*******************/

    CalculateStepperSpeed(&StepperMotor_x, -move_distance.x);
    CalculateStepperSpeed(&StepperMotor_y, -move_distance.y);
    /****************visualization(start)***************/

    OLED_ShowString(0, 0, "t:", OLED_8X16);
    OLED_ShowNum(5 * 6, 0, current_time, 8, OLED_8X16);
    OLED_ShowString(0, 0, "c_t:", OLED_8X16);
    OLED_ShowNum(5 * 6, 0, start_time, 8, OLED_8X16);

    OLED_ShowString(0, 2 * 16, "rect:", OLED_8X16);
    OLED_ShowSignedNum(0, 3 * 16, rect.x, 3, OLED_8X16);
    OLED_ShowSignedNum(0 + 5 * 6 + 7, 3 * 16, rect.y, 3, OLED_8X16);
    /****************visualization(end)*****************/
}

// id:1 任意视角锁目标，因本函数是阻塞式，因此需多次更新时间戳
void menu_mode_1(uint8_t cl_action)
{
    static uint8_t finding_flag = 1;
    static uint32_t mode2_start_time = 0;
    static uint32_t mode2_current_time = 0;
    static uint8_t mode2_start_flag = 0;

    // 更新时间戳
    mode2_current_time = uwTick;
    if (rect.x != 999 && rect.y != 999 && mode2_start_flag)
    {
        // 时间一到，便直接开启激光
        if (mode2_current_time - mode2_start_time >= 3000)
        {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); // Set the start pin to high
        }

        move_distance.x = rect.y - target_point.y;
        move_distance.y = rect.x - target_point.x;
        OLED_ShowNum(0, 2 * 16, mode2_current_time - mode2_start_time, 8, OLED_8X16);
    }
    else
    {
        move_distance.x = 0;
        move_distance.y = 0;
    }

    switch (cl_action)
    {
    case 0:
        // 使能标志位
        mode2_start_flag = 1;

        // 开始轮巡
        mode2_start_time = uwTick;

        while (rect.x == 999 && rect.y == 999 && finding_flag)
        {
            // 步进巡视，知道xy反了，别骂了别骂了
            CalculateStepperSpeed(&StepperMotor_y, -400); // 恒定400脉冲巡视
            // 处理数据包，更新坐标
            if (UART_GetPacket(&received_packet))
            {
                // 处理完整数据包（如转发或解析
                ParsePacket((char *)received_packet.data);
            }

            if (get_menu_action() == 3)
            {
                break; // 在循环中使能返回键，使其在开始模式后返回键依旧能用
            }

            // 更新时间戳计时
            mode2_current_time = uwTick;

            // 若没能巡视成功，便直接开启激光
            if (mode2_current_time - mode2_start_time >= 3500)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); // Set the start pin to high
            }

            // 可视化，便于调试
            OLED_ShowNum(0, 1 * 16, rect.x, 4, OLED_8X16);
            OLED_ShowNum(8 * 6, 1 * 16, rect.y, 4, OLED_8X16);
            OLED_Update();
        }
        finding_flag = 0;

        break;
    case 5: // 长按改变方向
        // 使能标志位
        mode2_start_flag = 1;
        // 开始轮巡
        mode2_start_time = uwTick;
        
        while (rect.x == 999 && rect.y == 999 && finding_flag)
        {
            // 步进巡视，知道xy反了，别骂了别骂了
            CalculateStepperSpeed(&StepperMotor_y, 400); // 恒定400脉冲巡视
            // 处理数据包，更新坐标
            if (UART_GetPacket(&received_packet))
            {
                // 处理完整数据包（如转发或解析
                ParsePacket((char *)received_packet.data);
            }

            if (get_menu_action() == 3)
            {
                break; // 在循环中使能返回键，使其在开始模式后返回键依旧能用
            }

            // 更新时间戳
            mode2_current_time = uwTick;

            // 若没能巡视成功，便直接开启激光
            if (mode2_current_time - mode2_start_time >= 3500)
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); // Set the start pin to high
            }

            // 可视化，便于调试
            OLED_ShowNum(0, 1 * 16, rect.x, 4, OLED_8X16);
            OLED_ShowNum(8 * 6, 1 * 16, rect.y, 4, OLED_8X16);
            OLED_Update();
        }
        finding_flag = 0;

        break;
    case 2:

        break;
    case 3:
        mode_inter_flag = 0; // return to menu
        finding_flag = 1;    // fing_flag
        mode2_current_time = 0;
        mode2_start_flag = 0;

        // if return ,stop the stepper and close the laser
        CalculateStepperSpeed(&StepperMotor_x, 0);
        CalculateStepperSpeed(&StepperMotor_y, 0);

        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Set the start pin to high
        break;
    default:
        break;
    }

    /****************function task(start)****************/

    CalculateStepperSpeed(&StepperMotor_x, -move_distance.x);
    CalculateStepperSpeed(&StepperMotor_y, -move_distance.y);

    /****************function task(end)******************/

    /****************visualization(start)***************/
    OLED_ShowString(0, 0, "finding", OLED_8X16);
    OLED_ShowNum(0, 1 * 16, rect.x, 4, OLED_8X16);
    OLED_ShowNum(8 * 6, 1 * 16, rect.y, 4, OLED_8X16);
    /****************visualization(end)*****************/
}

// id:2
void menu_mode_2(uint8_t cl_action)
{
    switch (cl_action)
    {
    case 0:

        break;
    case 1:

        break;
    case 2:

        break;
    case 3:
        mode_inter_flag = 0; // return to menu

    default:
        break;
    }
    /****************visualization(start)***************/

    /****************visualization(end)*****************/
}

// id:3
void menu_mode_3(uint8_t cl_action)
{
    switch (cl_action)
    {
    case 0:

        break;
    case 1:

        break;
    case 2:

        break;
    case 3:
        mode_inter_flag = 0; // return to menu

    default:
        break;
    }
    /****************visualization(start)***************/

    /****************visualization(end)*****************/
}

// id:4
void menu_mode_4(uint8_t cl_action)
{
    switch (cl_action)
    {
    case 0:

        break;
    case 1:

        break;
    case 2:

        break;
    case 3:
        mode_inter_flag = 0; // return to menu

    default:
        break;
    }
    /****************visualization(start)***************/

    /****************visualization(end)*****************/
}

// id:9 IMU angle display
void menu_mode_9(uint8_t cl_action)
{
    switch (cl_action)
    {
    case 0:
        break;
    case 1:

        break;
    case 2:

        break;
    case 3:
        mode_inter_flag = 0; // return to menu

    default:
        break;
    }
    /****************visualization(start)***************/
    OLED_ShowString(0, 0, "IMU Angle", OLED_8X16);
    OLED_ShowString(0, 1 * 16, "Roll:", OLED_8X16);
    OLED_ShowFloatNum(5 * 16, 1 * 16, Roll_copy, 3, 2, OLED_8X16);
    OLED_ShowString(0, 2 * 16, "Pitch:", OLED_8X16);
    OLED_ShowFloatNum(5 * 16, 2 * 16, Pitch_copy, 3, 2, OLED_8X16);
    OLED_ShowString(0, 3 * 16, "Yaw:", OLED_8X16);
    OLED_ShowFloatNum(5 * 16, 3 * 16, Yaw_copy, 3, 2, OLED_8X16);

    /****************visualization(end)*****************/
}

/******************************menu callback function(end)******************************/

void menu_system(void)
{
    static uint8_t cl_action = 0;  // action variable to store the menu action
    cl_action = get_menu_action(); // This function will clear the flag

    if (mode_inter_flag == 0)
    {
        current_menu = menu_navigate(current_menu, cl_action);
        OLED_DrawMenu(current_menu);
    }
    else
    {
        // hang your menu callback function here
        // handle the menu action by menu id
        switch (current_menu->id)
        {
        case 0:
            menu_mode_0(cl_action); // task 0
            break;
        case 1:
            menu_mode_1(cl_action); // task 1
            break;
        case 2:
            menu_mode_2(cl_action); // task 2
            break;
        case 3:
            menu_mode_3(cl_action); // task 3
            break;
        case 4:
            menu_mode_4(cl_action);
            break;
        case 9:
            menu_mode_9(cl_action); // IMU angle display
            break;
        default:
            // if the menu id is not defined, do nothing
            mode_inter_flag = 0; // reset the flag
            break;
        }
    }
}

// 清除指定按键的长按标志
void ClearLongPressFlag(uint8_t key_id)
{
    if (key_id < KEY_NUM)
    {
        keys[key_id].long_press_flag = 0;
    }
}

// 清除所有按键的长按标志
void ClearAllLongPressFlags(void)
{
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        keys[i].long_press_flag = 0;
    }
}
