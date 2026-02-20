#include <graphics.h> // 图形界面库, 需提前下载EasyX
#include <stdio.h>

#include "resource.h"

#include <time.h>     // 弄随机数的

#include <mmsystem.h>       // 包含多媒体设备接口
#pragma comment(lib, "winmm.lib")  // 加载多媒体库
// // 放嚎庭的bgm的~[doge]

#define N 5
#define RGB(r, g, b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))   // 了解到把这个宏加进来就可以使用RGB()来自定义颜色了, nice~!

int map[N];  // 存储黑块位置坐标
int score = 0; // 存储分数, 即点到的黑块的数量
double y_offset = 0;   // 当前下落的偏移距离
double speed = 2.0;    // 初始下落速度
bool clicked[4] = { false, false, false, false }; // 记录 0,1,2,3 行哪行点过了
void initBlack() {  // 随机产生黑块位置坐标
    for (int i = 0; i < 4; i++) { // 因为只有4行
        map[i] = rand() % N;   // 第i行(i=0~3)第map[i]列的矩形应该为黑块
    }
}

void drawBlack() {  // 根据map[]产生黑块
    for (int i = 0; i < 4; i++) {
        if (clicked[i]) {
            setfillcolor(LIGHTGRAY);  // 只要这一行点过了，就显示灰色
        }
        else {
            setfillcolor(BLACK);
        }
        fillrectangle(map[i] * 100, i * 120 + (int)y_offset, map[i] * 100 + 100, (i + 1) * 120 + (int)y_offset); 
    }
}

void draw() {
    BeginBatchDraw();  // 开始批量绘图

    cleardevice();  // 清屏

    // 设置分割线的颜色  要在绘制矩形格子前设置分割线颜色!
    setlinecolor(RGB(0,255,230));   // 黑色的分割线可以RGB(0, 0, 0)或者直接写BLACK
    setfillcolor(WHITE);
    // 下面绘制4行5列的格子, 即矩形
    // 绘制 5 行（多画一行作为缓冲，防止顶部出现空白）
    for (int i = -1; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            fillrectangle(j * 100, i * 120 + (int)y_offset, j * 100 + 100, i * 120 + 120 + (int)y_offset);   // 参数为左上角和右下角的坐标  绘制填充矩形(默认为白色)
        }
    }
    drawBlack();    // 根据map[]产生黑块
    EndBatchDraw();
}

bool play() {
    // 处理自动下落
    y_offset += speed;
    // 检查是否漏掉了黑块（黑块底部超过了屏幕底端）
    // 窗口高度是 480，最下面一行的黑块坐标超过 480 就算输
    if (y_offset >= 120) {
        // 判定：如果这一格滑过去了，但最下面那行没被点中，游戏结束
        if (!clicked[3]) {
            printf("漏掉了黑块！游戏结束\n");
            return true;
        }

        // 逻辑下移：数组数据后移
        for (int i = 3; i >= 1; i--) { 
            map[i] = map[i - 1];
            clicked[i] = clicked[i - 1]; // 点击状态跟着方块一起下移
        }
        
        map[0] = rand() % N;  // 在最上面一行随机产生一个新黑块
        clicked[0] = false; // 新生成的顶部块肯定是没点过的
        y_offset -= 120;   // 减去120，实现无缝平滑下落
        
    }
    
    if (MouseHit()) {
        MOUSEMSG msg; // MOUSEMSG是一个内置在EasyX中的结构体, 用来获取鼠标消息
        msg = GetMouseMsg();  // 获取鼠标消息
        switch (msg.uMsg) {
        case WM_LBUTTONDOWN:
            // msg.uMsg 就是在访问鼠标消息类型, 常见的有WM_LBUTTONDOWN 为 左键按下  WM_RBUTTONDOWN 为 右键按下
            // msg.x和msg.y可以访问到鼠标点击时的横纵坐标(均为int型)
            // 计算点击位置对应的逻辑行列 (需要减去 y_offset)
            int click_x = msg.x / 100;  // 100 是格子的宽度
            int click_y = (msg.y - (int)y_offset) / 120;  // 120 是格子的高度
            
            if (click_y >= 0 && click_y <= 3) {
                // 核心判定规则：
                // 1. 点中的必须是该行的黑块 (map[click_y])
                // 2. 该行还没被点过 (!clicked[click_y])
                // 3. 该行以下的所有行都必须已经点过了！

                bool lower_all_clicked = true;
                for (int k = click_y + 1; k <= 3; k++) {
                    if (!clicked[k]) {
                        lower_all_clicked = false;
                        break;
                    }
                }

                if (click_x == map[click_y] && !clicked[click_y] && lower_all_clicked) {
                    clicked[click_y] = true; // 记录这一行点过了
                    score += 1;
                    speed = 2.0 + (score / 4) * 0.7;
                    printf("蒸棒! 得分: %d\n", score);
                }
                else {
                    // 如果点的是已经点过的，或者跳着点（下面还没点完就点上面），或者点错
                    printf("点错啦！\n");
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

void PlayBGM() {
    // SND_RESOURCE: 从资源加载
    // SND_ASYNC: 异步播放，不卡程序
    // SND_LOOP: 循环播放
    PlaySound(MAKEINTRESOURCE(IDR_BGM), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
}

void setIcon() {
    // 设置窗口左上角图标
    // 获取当前窗口的句柄 (HWND)
    HWND hwnd = GetHWnd();
    // 从资源中加载图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    // 发送消息给窗口，设置大图标（Alt+Tab时看到）和小图标（标题栏看到）
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

void CleanUp() {   // 这个函数负责在游戏结束时打扫战场
    // 停止并关闭音乐设备
    // 停止当前正在播放的所有 PlaySound 声音
    PlaySound(NULL, NULL, 0);
}

int main()
{
    srand((unsigned)time(NULL)); // 时间作随机数的种子

    // 创建窗口界面 默认背景色为黑色
    initgraph(500, 480);  // 宽和高 (单位px)  原点为界面左上角

    setIcon();            // 让玩游戏时，任务栏和窗口左上角都会显示图标
    
    initBlack();    // 随机产生黑块位置坐标
    y_offset = 0;

    // 确认弹窗
    // MB_OKCANCEL 会显示“确定”和“取消”按钮
    int ret = MessageBox(GetHWnd(), "准备好了吗？点击确定开始3秒倒计时！", "游戏提示", MB_OKCANCEL | MB_ICONQUESTION);
    if (ret == IDCANCEL) {
        closegraph(); // 关闭绘图窗口
        return 0; // 用户点取消，直接退出
    }

    // 3秒倒计时
    settextcolor(RGB(0, 255, 230));           // 设置文字颜色
    settextstyle(60, 0, "微软雅黑"); // 设置文字大小和字体 settextstyle(高度, 宽度, 字体)
    setbkmode(TRANSPARENT); // 设置文字背景透明，否则文字后面会有个白框遮挡格子

    for (int i = 3; i >= 0; i--) {
        // 先画出静态的游戏背景和黑块
        draw();

        char s[5];
        if (i > 0) sprintf_s(s, sizeof(s), "%d", i);   // 将数字转为字符串
        else sprintf_s(s, sizeof(s), "GO!"); // 最后一秒显示 GO

        // 在屏幕中心绘制倒计时数字
        // 居中计算：(窗口宽/2 - 字符宽/2, 窗口高/2 - 字符高/2)
        outtextxy(i > 0 ? 230 : 180, 180, s); // GO 比较宽，坐标稍微左移 // outtextxy(x, y, 字符串)：在指定的坐标位置输出文字。由于窗口宽 500，高 480，我们把数字放在 (230, 200) 附近大致就是视觉中心。

        Sleep(1000); // 这个函数会让程序整整“睡” 1000 毫秒（1秒），这是实现精确秒级倒计时的最简单办法。
    }

    // 开始游戏前清空倒计时期间误点的鼠标消息
    FlushMouseMsgBuffer();

    PlayBGM();     // 播放背景音乐
 
    while (1) {
        draw();         // 绘制4行5列的格子

        if (play()) {
            char str[128]; 
            sprintf_s(str, "总计得分为%d", score);

            CleanUp();    // 游戏结束后的处理

            MessageBox(GetHWnd(), str, "Game Over!", MB_OK);  // MessageBox(窗口句柄,消息文本,对话框标题,对话框样式)用于创建和显示一个消息对话框, MB_OK 即含 确定按钮 MB_OKCANCEL 即含 确定和取消按钮
            exit(0);  // 结束程序
        }
        // 控制刷新频率
        // Sleep(10) 代表每秒刷新约 100 次, 控制帧率, 保证画面流畅且不吃满 CPU
        Sleep(10);
    }

    // 卡屏
    getchar();
    return 0;
}
