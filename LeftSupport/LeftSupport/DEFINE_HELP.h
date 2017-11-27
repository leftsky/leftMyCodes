

#ifndef DEFINE_HELP
#define DEFINE_HELP
/*
* Class styles
*/
/*������ڵ�λ�û�߶ȸı䣬���ػ洰��*/
#define CS_VREDRAW          0x0001
/*������ڵ�λ�û��ȷ����ı䣬���ػ洰��*/
#define CS_HREDRAW          0x0002
/*���û�˫������ʱ�����򴰿ں����������˫����Ϣ*/
#define CS_DBLCLKS          0x0008
/*Ϊͬһ���������е�ÿ�����ڴ���һ��Ψһ���豸������*/
#define CS_OWNDC            0x0020
/*����һ���豸�����������е����д��干��
���ǿ���������һ��Ӧ�ó���������̴߳�����һ����ͬ��Ĵ��塣
������߳���ͼͬʱʹ����ͬ���豸����ʱ��ϵͳֻ����һ���̳߳ɹ��ؽ��л�ͼ����*/
#define CS_CLASSDC          0x0040
/*�����Ӵ����м��µľ������򵽸������У���ʹ�Ӵ��ڿ����ڸ������ϻ�ͼ��
ָ���÷��������Ӧ�ó��������*/
#define CS_PARENTDC         0x0080
/*�����еġ��رա���ť������*/
#define CS_NOCLOSE          0x0200
/*�ѱ��������ڵ���Ļͼ����Ϊλͼ����������
���ô��ڱ��ƶ�ʱ��Windows����ϵͳʹ�ñ������λͼ���ؽ���Ļͼ��*/
#define CS_SAVEBITS         0x0800
/*���ڵĿͻ������ԡ��ַ��߽硱���룬
��ϵͳ�������ڵ�ˮƽλ��ʱ���ͻ���������������8��������*/
#define CS_BYTEALIGNCLIENT  0x1000
/*�����ԡ��ַ��߽硱���룬
��ϵͳ�������ڵ�ˮƽλ��ʱ���ͻ���������������8��������*/
#define CS_BYTEALIGNWINDOW  0x2000
/*ָ���˴�������һ��Ӧ�ó���ȫ���ࡣ
Ӧ�ó���ȫ��������һ���ڽ����ж�����ģ����Ч��exe��dllע��Ĵ�����*/
#define CS_GLOBALCLASS      0x4000
/*δ֪*/
#define CS_IME              0x00010000
#if(_WIN32_WINNT >= 0x0501)
/*������ӰЧ��*/
#define CS_DROPSHADOW       0x00020000
#endif /* _WIN32_WINNT >= 0x0501 */



/*
* Window Styles
*/
/*����һ���ص����ڡ��ص�����ͨ�����б������ͱ߽硣*/
#define WS_OVERLAPPED       0x00000000L
/*����һ������ʽ���ڣ�������WS_CHILD���һ��ʹ�á�*/
#define WS_POPUP            0x80000000L
/*˵������Ϊ�Ӵ��ڣ�����Ӧ���ڵ���ʽ���ڷ��(WS_POPUP)��*/
#define WS_CHILD            0x40000000L
/*����һ����ʼ״̬Ϊ��С���Ĵ��ڡ�����WS_OVERLAPPED���һ��ʹ�á�*/
#define WS_MINIMIZE         0x20000000L
/*����һ������ɼ��Ĵ��ڡ�*/
#define WS_VISIBLE          0x10000000L
/*����һ����ʼ״̬Ϊ��ֹ�Ĵ��ڡ�*/
#define WS_DISABLED         0x08000000L
/*������ص��Ӵ��ڣ�����ζ�ţ���һ���ض����Ӵ��ڽ��յ��ػ���Ϣʱ��
WS_CLIPSIBLINGS������Ӵ���Ҫ�ػ���������ȥ���������Ӵ����ص��Ĳ��֡�
�����û��ָ��WS_CLIPSIBLINGS��񣬲����Ӵ������ص���������һ���Ӵ��ڵĿͻ�����ͼʱ��
�����ܻử�����ڵ��Ӵ��ڵĿͻ����С���ֻ��WS_CHILD���һ��ʹ�á�*/
#define WS_CLIPSIBLINGS     0x04000000L
/*���Ƹ�����ʱ���������Ӵ��ڵĲü�����ʹ���ڽ���������ʱ��*/
#define WS_CLIPCHILDREN     0x02000000L
/*����һ����󻯵Ĵ��ڡ�*/
#define WS_MAXIMIZE         0x01000000L
/*�����WS_BORDER�����ϣ���������WS_DLGFRAME���һ��ʹ�á�
ָʾ���ڰ�������Ҫ���֡�
�������ԣ����е�WS_CAPTION���Ƕ���ģ�
��Ϊ������WS_BORDER��WS_DLGFRAME��ɵģ�ȥ�����Դ���û���κ�Ӱ�죩
WS_BORDER | WS_DLGFRAME
*/
#define WS_CAPTION          0x00C00000L
/*�б߿򴰿�*/
#define WS_BORDER           0x00800000L
/*����һ�����ڣ�����˫�ر߽磬����û�б�������*/
#define WS_DLGFRAME         0x00400000L
/*����һ�����д�ֱ�������Ĵ��ڡ�*/
#define WS_VSCROLL          0x00200000L
/*����һ������ˮƽ�������Ĵ��ڡ�*/
#define WS_HSCROLL          0x00100000L
/*����һ���ڱ������Ͼ��п��Ʋ˵��Ĵ��ڡ����Դ��������Ĵ���ʹ�á�
(WS_SYSTMENU�����˴����Ƿ���ϵͳ�˵�����Ȼ���б������Ĵ����У�
�����־�������˴������Ͻǵ�Сͼ���Ƿ���ڣ��Լ����Ͻǵİ�ť�Ƿ���ڣ��رհ�ť��)*/
#define WS_SYSMENU          0x00080000L
/*����һ�����к�߿�Ĵ��ڣ�����ͨ����߿����ı䴰�ڴ�С��
{WS_THICKFRAME�����˴��ڱ�Ե�Ƿ������ק}WS_TILED������һ������Ĵ��ڡ�
һ������Ĵ�����һ�������һ���߿���WS_OVERLAPPED�����ͬ��*/
#define WS_THICKFRAME       0x00040000L
/*ָ��һ��ؼ��еĵ�һ�����û������ü�ͷ��������ؼ����ƶ���
�ڵ�һ���ؼ������WS_GROUP�������ΪFALSE�Ŀؼ���������һ�顣
��һ������WS_GROUP���Ŀؼ�����ʼ��һ�飨����ζ��һ��������һ��Ŀ�ʼ��������*/
#define WS_GROUP            0x00020000L
/*ָ����һЩ�ؼ��е�һ�����û�����ͨ��TAB�����ƹ�����
TAB��ʹ�û��ƶ�����һ����WS_TABSTOP�����Ŀؼ���*/
#define WS_TABSTOP          0x00010000L
/*����һ��������С����ť�Ĵ��ڡ�
��WS_MINIMIZEBOX��WS_MAXIMIZEBOX�������ϵͳ�˵��е���С����
����Ƿ���ã��Լ��������Ͻ��Ƿ�����󻯺���С����ť��
��Ȼ��������־������WS_SYSMENU���ڵ�����²���Ч��
������û��ϵͳ�˵����������Ͻ��ǲ�������κΰ�ť�ġ���
*/
#define WS_MINIMIZEBOX      0x00020000L
/*����һ��������󻯰�ť�Ĵ��ڡ�*/
#define WS_MAXIMIZEBOX      0x00010000L

/*WS_OVERLAPPED*/
#define WS_TILED            WS_OVERLAPPED
/*WS_MINIMIZE*/
#define WS_ICONIC           WS_MINIMIZE
/*WS_THICKFRAME*/
#define WS_SIZEBOX          WS_THICKFRAME
/*WS_OVERLAPPEDWINDOW*/
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW

/*
* Common Window Styles
*/
/*����һ������WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, 
WS_THICKFRAME, WS_MINIMIZEBOX��WS_MAXIMIZEBOX�����ص�ʽ���ڡ�*/
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)
/*����һ������WS_BORDER��WS_POPUP��WS_SYSMENU���ĵ������ڡ�
Ϊ��ʹ���Ʋ˵��ɼ���������WS_POPUPWINDOWһ��ʹ��WS_CAPTION���*/
#define WS_POPUPWINDOW      (WS_POPUP          | \
                             WS_BORDER         | \
                             WS_SYSMENU)
/*WS_CHILD*/
#define WS_CHILDWINDOW      (WS_CHILD)

/*
* Extended Window Styles
*/
/*ָ��һ������˫�ر߽�Ĵ��ڣ�
������dwStyle������ָ����WS_CAPTION����־ʱ�������Ծ��б���������ѡ����*/
#define WS_EX_DLGMODALFRAME     0x00000001L
/*ָ���������񴴽����Ӵ����ڱ����������ٵ�ʱ�򽫲��򸸴��ڷ���WM_PARENTNOTIFY��Ϣ��*/
#define WS_EX_NOPARENTNOTIFY    0x00000004L
/*ָ���������񴴽��Ĵ��ڱ��뱻�������зǶ��㴰�ڵ����棬
��ʹ��������Ѿ������ڼ���״̬�������Ǳ����������档
Ӧ�ó��������SetWindowsPos��Ա�����������ȥ��������ԡ�*/
#define WS_EX_TOPMOST           0x00000008L
/*ָ���������񴴽��Ĵ����ܹ������Ϸ��ļ���*/
#define WS_EX_ACCEPTFILES       0x00000010L
/*ָ�����������񴴽��Ĵ�����͸���ġ�
����ζ�ţ����������������κδ��ڶ����ᱻ������ڵ�ס��
�������񴴽��Ĵ���ֻ�е�������Ĵ��ڶ����¹��Ժ�Ž���WM_PAINT��Ϣ��*/
#define WS_EX_TRANSPARENT       0x00000020L
#if(WINVER >= 0x0400)
/*����һ��MDI�Ӵ��ڡ�*/
#define WS_EX_MDICHILD          0x00000040L
/*����һ�����ߴ��ڣ�Ŀ���Ǳ�����������������
���ߴ��ھ��б���������ͨ���ı�����Ҫ�̣����ڵı�������С������ʾ�ġ�
���ߴ��ڲ����������������û�����ALT + TABʱ���ֵĴ����С�*/
#define WS_EX_TOOLWINDOW        0x00000080L
/*ָ���˾���͹��߿�Ĵ���*/
#define WS_EX_WINDOWEDGE        0x00000100L
/*ָ�����ھ���3D��ۣ�����ζ�ţ��߿�����³��ı߽硣*/
#define WS_EX_CLIENTEDGE        0x00000200L
/*�ڴ��ڵı������а����ʺš����û������ʺ�ʱ����������״��Ϊ��ָ����ʺš�
����û���󵥻�һ���Ӵ��ڣ��Ӵ��ڽ����յ�һ��WM_HELP��Ϣ��*/
#define WS_EX_CONTEXTHELP       0x00000400L

#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0400)
/*���贰���Ҷ������ԡ����봰�����йء�*/
#define WS_EX_RIGHT             0x00001000L
/*ָ�����ھ�����������ԡ�����ȱʡֵ��*/
#define WS_EX_LEFT              0x00000000L
/*���մ��ҵ����˳����ʾ�����ı���*/
#define WS_EX_RTLREADING        0x00002000L
/*���մ����ҵķ�ʽ��ʾ�����ı�������ȱʡ��ʽ��*/
#define WS_EX_LTRREADING        0x00000000L
/*����ֱ���������ڿͻ�������ߡ�*/
#define WS_EX_LEFTSCROLLBAR     0x00004000L
/*����ֱ������������У����ڿͻ������ұߡ�����ȱʡ��ʽ��*/
#define WS_EX_RIGHTSCROLLBAR    0x00000000L
/*�����û���TAB���������ڵ��Ӵ��ڡ�*/
#define WS_EX_CONTROLPARENT     0x00010000L
/*����һ��������ά�߽�Ĵ��ڣ����ڲ������û�������*/
#define WS_EX_STATICEDGE        0x00020000L
/*�����ڿɼ�ʱ��һ�����㴰�ڷ������������ϡ�*/
#define WS_EX_APPWINDOW         0x00040000L

/*WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE*/
#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
/*WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST*/
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)

#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
/*������ʹ����֧��͸������һ���ֲ�Ĵ���.
ע��,�ⲻ�������Ӵ�����.ͬ��,������ھ���CS_OWNDC ,CS_CLASSDC��ʽ,��Ҳ����ʹ��.*/
#define WS_EX_LAYERED           0x00080000

#endif /* _WIN32_WINNT >= 0x0500 */


#if(WINVER >= 0x0500)
/*�ô˲��������Ĵ��岻�ᴫ�����Ĵ��ڲ��ָ������Ӵ���
Disable inheritence of mirroring by children*/
#define WS_EX_NOINHERITLAYOUT   0x00100000L
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0602)
/*���ڲ������ض���ı��档
������Ϊwindowsû�пɼ����ݻ�ʹ�ñ�������Ļ������ṩ���ǵ��Ӿ���*/
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif /* WINVER >= 0x0602 */

#if(WINVER >= 0x0500)
/*�������Լ��������汾��98/ME,2000/XP����һ��ˮƽ������ұߵĴ���.
Խ�����ˮƽ����ֵ���.Right to left mirroring*/
#define WS_EX_LAYOUTRTL         0x00400000L
#endif /* WINVER >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)
/*��һ������������Ӵ���ʹ��˫���尴�մӵ͵��߷�ʽ���Ƴ���,����remark��.
�������Ӵ��Ѿ�ʹ�þ�����ʽ�е�����ֵCS_OWNDC , CS_CLASSDC,WS_EX_CONTEXTHELP.�˲���������ʹ��.
�����ʽ���Ӵ��ڱ���������һ���ʺ�,��ӵ������Ÿ��ʺ�,�����һ���ʺ�,
����û�Ȼ����һ���Ӵ���,�Ӵ��ͻ��յ�һ��WM_HELP��Ϣ.
�Ӵ��ڽ��������Ϣ���ݸ����ĸ�����,��������̽���HELP_WM_HELP�������WinHelp����.
����������򳣳�����һ�����͵İ������Ӵ��ڵİ����Ĵ���
���������ܺ�WS_MAXIMIZEBOX ,WS_MINIMIZEBOXһ��ʹ��*/
#define WS_EX_COMPOSITED        0x02000000L
#endif /* _WIN32_WINNT >= 0x0501 */
#if(_WIN32_WINNT >= 0x0500)
/*һ��ʹ�ô˲��������Ķ������ڲ�����ǰ̨����,
���û������ʱ.ϵͳ���Ὣ�˴��ڷŵ�ǰ̨,���û���С�����߹ر����ǰ̨����.
Ҫ���������Ĵ���,ʹ��SetActiveWindow����SetForegroundWindow����
�����͵Ĵ���Ĭ�ϲ�����ʾ����������.Ҫǿ�н������Ĵ�����ʾ����������,ʹ��WS_EX_APPWINDOW����.*/
#define WS_EX_NOACTIVATE        0x08000000L
#endif /* _WIN32_WINNT >= 0x0500 */

#endif


