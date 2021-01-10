#ifndef CMD_GLOBLE_HEAD_FILE
#define CMD_GLOBLE_HEAD_FILE
//公共消息定义
#define SUB_WINDOW_OPEN							(WM_USER+100)				//窗口建立消息
#define SUB_WINDOW_ACTIVE							(WM_USER+101)				//窗口获取焦点
#define SUB_WINDOW_MOVE							(WM_USER+102)				//窗口移动
#define SUB_WINDOW_DESTORY						(WM_USER+103)				//窗口销毁
#define SUB_KEY_TYPE										(WM_USER+104)				//按键消息
#define SUB_MOUSE_CLICK								(WM_USER+105)				//鼠标点击消息
#define SUB_MAX_MIN										(WM_USER+106)				//最大最小化
#define SUB_SET_TEXT										(WM_USER+107)				//设置文本
#define SUB_UPDATE_CLOSE							(WM_USER+108)				//更新完成，关闭程序
#define SUB_START_UPDATE								(WM_USER+109)				//开始更新

//#define Mouse_Span										0.05f								//鼠标事件权值
//#define Key_Span											0.4f								//键盘事件权值
//#define Area_Span											2.5f								//窗口面积权值
const float Mouse_Span = 0.05f;																//鼠标事件权值
const float Key_Span	 = 0.4f;																		//键盘事件权值										
const float Area_Span = 2.5f;																	//窗口面积权值

#define DB_LOG																					//开启数据库记录
#define DATA_CACHE																			//开启数据内存缓存

#ifdef DB_LOG
//#define Fail_Threshold									5									//连接失效5号后断开连接
const int Fail_Threshold = 5;		//连接失效N次后重连
#endif
#endif