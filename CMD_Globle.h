#ifndef CMD_GLOBLE_HEAD_FILE
#define CMD_GLOBLE_HEAD_FILE
//������Ϣ����
#define SUB_WINDOW_OPEN							(WM_USER+100)				//���ڽ�����Ϣ
#define SUB_WINDOW_ACTIVE							(WM_USER+101)				//���ڻ�ȡ����
#define SUB_WINDOW_MOVE							(WM_USER+102)				//�����ƶ�
#define SUB_WINDOW_DESTORY						(WM_USER+103)				//��������
#define SUB_KEY_TYPE										(WM_USER+104)				//������Ϣ
#define SUB_MOUSE_CLICK								(WM_USER+105)				//�������Ϣ
#define SUB_MAX_MIN										(WM_USER+106)				//�����С��
#define SUB_SET_TEXT										(WM_USER+107)				//�����ı�
#define SUB_UPDATE_CLOSE							(WM_USER+108)				//������ɣ��رճ���
#define SUB_START_UPDATE								(WM_USER+109)				//��ʼ����

//#define Mouse_Span										0.05f								//����¼�Ȩֵ
//#define Key_Span											0.4f								//�����¼�Ȩֵ
//#define Area_Span											2.5f								//�������Ȩֵ
const float Mouse_Span = 0.05f;																//����¼�Ȩֵ
const float Key_Span	 = 0.4f;																		//�����¼�Ȩֵ										
const float Area_Span = 2.5f;																	//�������Ȩֵ

#define DB_LOG																					//�������ݿ��¼
#define DATA_CACHE																			//���������ڴ滺��

#ifdef DB_LOG
//#define Fail_Threshold									5									//����ʧЧ5�ź�Ͽ�����
const int Fail_Threshold = 5;		//����ʧЧN�κ�����
#endif
#endif