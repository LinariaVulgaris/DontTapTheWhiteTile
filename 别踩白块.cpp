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

int mapx, mapy;  // 存储鼠标点击处所属于的格子的下标

int score = 0; // 存储分数, 即点到的黑块的数量

void initBlack() {  // 随机产生黑块位置坐标
    for (int i = 0; i < 4; i++) { // 因为只有4行
        map[i] = rand() % N;   // 第i行(i=0~3)第map[i]列的矩形应该为黑块
    }
}

void drawBlack() {  // 根据map[]产生黑块
    setfillcolor(BLACK);
    for (int i = 0; i < 4; i++) {
        fillrectangle(map[i] * 100, i * 120, map[i] * 100 + 100, i * 120 + 120);
    }

}

void draw() {
    BeginBatchDraw();  // 开始批量绘图

    cleardevice();  // 清屏

    // 设置分割线的颜色  要在绘制矩形格子前设置分割线颜色!
    setlinecolor(RGB(255,0,0));   // 黑色的分割线可以RGB(0, 0, 0)或者直接写BLACK
    setfillcolor(WHITE);
    // 下面绘制4行5列的格子, 即矩形
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            fillrectangle(j * 100, i * 120, j * 100 + 100, i * 120 + 120);   // 参数为左上角和右下角的坐标  绘制填充矩形(默认为白色)
        }
    }
    drawBlack();    // 根据map[]产生黑块
    EndBatchDraw();
}

bool play() {
    MOUSEMSG msg; // MOUSEMSG是一个内置在EasyX中的结构体, 用来获取鼠标消息
    msg = GetMouseMsg();  // 获取鼠标消息
    switch (msg.uMsg) {
    case WM_LBUTTONDOWN:
        // msg.uMsg 就是在访问鼠标消息类型, 常见的有WM_LBUTTONDOWN 为 左键按下  WM_RBUTTONDOWN 为 右键按下
        // msg.x和msg.y可以访问到鼠标点击时的横纵坐标(均为int型)
        mapx = msg.x / 100;  // 100 是格子的宽度
        mapy = msg.y / 120;  // 120 是格子的高度
        // 即鼠标点到了第mapy行, 第mapx列的格子(行范围为0~3, 列范围为0~4)
        if (map[3] == mapx && mapy == 3) {  // 这个游戏只能点最下面一行即第三行的黑块来消除, 所以该语句相当于if(点到了黑块)
            for (int i = 3; i >= 1; i--) map[i] = map[i - 1];  // 黑块下移
            map[0] = rand() % N;  // 在最上面一行随机产生一个新黑块
            score += 1;
            printf("真棒!\n");
        }
        else {
            printf("游戏结束~\n");
            return true;
        }
        break;
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
    
    PlayBGM();     // 播放背景音乐

    initBlack();    // 随机产生黑块位置坐标
 
    while (1) {
        draw();         // 绘制4行5列的格子
        
        if (play()) {
            char str[128]; 
            sprintf_s(str, "总计得分为%d", score);

            CleanUp();    // 游戏结束后的处理

            MessageBox(GetHWnd(), str, "Game Over!", MB_OK);  // MessageBox(窗口句柄,消息文本,对话框标题,对话框样式)用于创建和显示一个消息对话框, MB_OK 即含 确定按钮 MB_OKCANCEL 即含 确定和取消按钮
            exit(0);  // 结束程序
        }
    }

    // 卡屏
    getchar();
    return 0;
}


