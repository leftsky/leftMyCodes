

#ifndef DEFINE_HELP
#define DEFINE_HELP
/*
* Class styles
*/
/*如果窗口的位置或高度改变，将重绘窗口*/
#define CS_VREDRAW          0x0001
/*如果窗口的位置或宽度发生改变，将重绘窗口*/
#define CS_HREDRAW          0x0002
/*当用户双击窗口时，将向窗口函数发送鼠标双击消息*/
#define CS_DBLCLKS          0x0008
/*为同一个窗口类中的每个窗口创建一个唯一的设备上下文*/
#define CS_OWNDC            0x0020
/*分配一个设备环境并被类中的所有窗体共享。
它是可以适用于一个应用程序的若干线程创建的一个相同类的窗体。
当多个线程试图同时使用相同的设备环境时，系统只允许一个线程成功地进行绘图操作*/
#define CS_CLASSDC          0x0040
/*设置子窗口中剪下的矩形区域到父窗口中，以使子窗口可以在父窗口上绘图。
指定该风格可以提高应用程序的性能*/
#define CS_PARENTDC         0x0080
/*窗口中的“关闭”按钮不可用*/
#define CS_NOCLOSE          0x0200
/*把被窗口遮掩的屏幕图像作为位图保存起来。
当该窗口被移动时，Windows操作系统使用被保存的位图来重建屏幕图像*/
#define CS_SAVEBITS         0x0800
/*窗口的客户区域以“字符边界”对齐，
当系统调整窗口的水平位置时，客户区域的左边坐标是8的整数倍*/
#define CS_BYTEALIGNCLIENT  0x1000
/*窗口以“字符边界”对齐，
当系统调整窗口的水平位置时，客户区域的左边坐标是8的整数倍*/
#define CS_BYTEALIGNWINDOW  0x2000
/*指定此窗体类是一个应用程序全局类。
应用程序全局类是由一个在进程中对所有模块有效的exe或dll注册的窗体类*/
#define CS_GLOBALCLASS      0x4000
/*未知*/
#define CS_IME              0x00010000
#if(_WIN32_WINNT >= 0x0501)
/*窗口阴影效果*/
#define CS_DROPSHADOW       0x00020000
#endif /* _WIN32_WINNT >= 0x0501 */



/*
* Window Styles
*/
/*创建一个重叠窗口。重叠窗口通常具有标题条和边界。*/
#define WS_OVERLAPPED       0x00000000L
/*创建一个弹出式窗口，不能与WS_CHILD风格一起使用。*/
#define WS_POPUP            0x80000000L
/*说明窗口为子窗口，不能应用于弹出式窗口风格(WS_POPUP)。*/
#define WS_CHILD            0x40000000L
/*创建一个初始状态为最小化的窗口。仅与WS_OVERLAPPED风格一起使用。*/
#define WS_MINIMIZE         0x20000000L
/*创建一个最初可见的窗口。*/
#define WS_VISIBLE          0x10000000L
/*创建一个初始状态为禁止的窗口。*/
#define WS_DISABLED         0x08000000L
/*剪裁相关的子窗口，这意味着，当一个特定的子窗口接收到重绘消息时，
WS_CLIPSIBLINGS风格将在子窗口要重画的区域中去掉与其它子窗口重叠的部分。
（如果没有指定WS_CLIPSIBLINGS风格，并且子窗口有重叠，当你在一个子窗口的客户区绘图时，
它可能会画在相邻的子窗口的客户区中。）只与WS_CHILD风格一起使用。*/
#define WS_CLIPSIBLINGS     0x04000000L
/*绘制父窗口时，不绘制子窗口的裁剪区域。使用在建立父窗口时。*/
#define WS_CLIPCHILDREN     0x02000000L
/*创建一个最大化的窗口。*/
#define WS_MAXIMIZE         0x01000000L
/*必须和WS_BORDER风格配合，但不能与WS_DLGFRAME风格一起使用。
指示窗口包含标题要部分。
（很明显，其中的WS_CAPTION就是多余的，
因为它是由WS_BORDER和WS_DLGFRAME组成的，去掉它对窗口没有任何影响）
WS_BORDER | WS_DLGFRAME
*/
#define WS_CAPTION          0x00C00000L
/*有边框窗口*/
#define WS_BORDER           0x00800000L
/*创建一个窗口，具有双重边界，但是没有标题条。*/
#define WS_DLGFRAME         0x00400000L
/*创建一个具有垂直滚动条的窗口。*/
#define WS_VSCROLL          0x00200000L
/*创建一个具有水平滚动条的窗口。*/
#define WS_HSCROLL          0x00100000L
/*创建一个在标题条上具有控制菜单的窗口。仅对带标题条的窗口使用。
(WS_SYSTMENU决定了窗口是否有系统菜单，当然在有标题栏的窗口中，
这个标志还决定了窗口左上角的小图标是否存在，以及右上角的按钮是否存在（关闭按钮）)*/
#define WS_SYSMENU          0x00080000L
/*创建一个具有厚边框的窗口，可以通过厚边框来改变窗口大小。
{WS_THICKFRAME决定了窗口边缘是否可以拖拽}WS_TILED：产生一个层叠的窗口。
一个层叠的窗口有一个标题和一个边框。与WS_OVERLAPPED风格相同。*/
#define WS_THICKFRAME       0x00040000L
/*指定一组控件中的第一个，用户可以用箭头键在这组控件中移动。
在第一个控件后面把WS_GROUP风格设置为FALSE的控件都属于这一组。
下一个具有WS_GROUP风格的控件将开始下一组（这意味着一个组在下一组的开始处结束）*/
#define WS_GROUP            0x00020000L
/*指定了一些控件中的一个，用户可以通过TAB键来移过它。
TAB键使用户移动到下一个用WS_TABSTOP风格定义的控件。*/
#define WS_TABSTOP          0x00010000L
/*创建一个具有最小化按钮的窗口。
（WS_MINIMIZEBOX和WS_MAXIMIZEBOX则决定了系统菜单中的最小化，
最大化是否可用，以及窗口右上角是否有最大化和最小化按钮，
当然这两个标志必须在WS_SYSMENU存在的情况下才有效，
否则是没有系统菜单，而且右上角是不会出现任何按钮的。）
*/
#define WS_MINIMIZEBOX      0x00020000L
/*创建一个具有最大化按钮的窗口。*/
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
/*创建一个具有WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, 
WS_THICKFRAME, WS_MINIMIZEBOX和WS_MAXIMIZEBOX风格的重叠式窗口。*/
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)
/*创建一个具有WS_BORDER，WS_POPUP和WS_SYSMENU风格的弹出窗口。
为了使控制菜单可见，必须与WS_POPUPWINDOW一起使用WS_CAPTION风格。*/
#define WS_POPUPWINDOW      (WS_POPUP          | \
                             WS_BORDER         | \
                             WS_SYSMENU)
/*WS_CHILD*/
#define WS_CHILDWINDOW      (WS_CHILD)

/*
* Extended Window Styles
*/
/*指明一个具有双重边界的窗口，
当你在dwStyle参数中指定了WS_CAPTION风格标志时，它可以具有标题条（可选）。*/
#define WS_EX_DLGMODALFRAME     0x00000001L
/*指定用这个风格创建的子窗口在被创建或销毁的时候将不向父窗口发送WM_PARENTNOTIFY消息。*/
#define WS_EX_NOPARENTNOTIFY    0x00000004L
/*指定用这个风格创建的窗口必须被放在所有非顶层窗口的上面，
即使这个窗口已经不处于激活状态，它还是保留在最上面。
应用程序可以用SetWindowsPos成员函数来加入或去掉这个属性。*/
#define WS_EX_TOPMOST           0x00000008L
/*指明用这个风格创建的窗口能够接受拖放文件。*/
#define WS_EX_ACCEPTFILES       0x00000010L
/*指定了用这个风格创建的窗口是透明的。
这意味着，在这个窗口下面的任何窗口都不会被这个窗口挡住。
用这个风格创建的窗口只有当它下面的窗口都更新过以后才接收WM_PAINT消息。*/
#define WS_EX_TRANSPARENT       0x00000020L
#if(WINVER >= 0x0400)
/*创建一个MDI子窗口。*/
#define WS_EX_MDICHILD          0x00000040L
/*创建一个工具窗口，目的是被用作浮动工具条。
工具窗口具有标题条，比通常的标题条要短，窗口的标题是用小字体显示的。
工具窗口不出现在任务条或用户按下ALT + TAB时出现的窗口中。*/
#define WS_EX_TOOLWINDOW        0x00000080L
/*指定了具有凸起边框的窗口*/
#define WS_EX_WINDOWEDGE        0x00000100L
/*指明窗口具有3D外观，这意味着，边框具有下沉的边界。*/
#define WS_EX_CLIENTEDGE        0x00000200L
/*在窗口的标题条中包含问号。当用户单击问号时，鼠标光标的形状变为带指针的问号。
如果用户随后单击一个子窗口，子窗口将接收到一个WM_HELP消息。*/
#define WS_EX_CONTEXTHELP       0x00000400L

#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0400)
/*赋予窗口右对齐属性。这与窗口类有关。*/
#define WS_EX_RIGHT             0x00001000L
/*指定窗口具有左对齐属性。这是缺省值。*/
#define WS_EX_LEFT              0x00000000L
/*按照从右到左的顺序显示窗口文本。*/
#define WS_EX_RTLREADING        0x00002000L
/*按照从左到右的方式显示窗口文本。这是缺省方式。*/
#define WS_EX_LTRREADING        0x00000000L
/*将垂直滚动条放在客户区的左边。*/
#define WS_EX_LEFTSCROLLBAR     0x00004000L
/*将垂直滚动条（如果有）放在客户区的右边。这是缺省方式。*/
#define WS_EX_RIGHTSCROLLBAR    0x00000000L
/*允许用户用TAB键遍历窗口的子窗口。*/
#define WS_EX_CONTROLPARENT     0x00010000L
/*创建一个具有三维边界的窗口，用于不接受用户输入的项。*/
#define WS_EX_STATICEDGE        0x00020000L
/*当窗口可见时将一个顶层窗口放置在任务栏上。*/
#define WS_EX_APPWINDOW         0x00040000L

/*WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE*/
#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
/*WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST*/
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)

#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
/*该属性使窗体支持透明创建一个分层的窗口.
注意,这不能用在子窗口上.同样,如果窗口具有CS_OWNDC ,CS_CLASSDC样式,这也不用使用.*/
#define WS_EX_LAYERED           0x00080000

#endif /* _WIN32_WINNT >= 0x0500 */


#if(WINVER >= 0x0500)
/*用此参数创建的窗体不会传递他的窗口布局给他的子窗口
Disable inheritence of mirroring by children*/
#define WS_EX_NOINHERITLAYOUT   0x00100000L
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0602)
/*窗口不呈现重定向的表面。
这是因为windows没有可见内容或使用表面以外的机制来提供他们的视觉。*/
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif /* WINVER >= 0x0602 */

#if(WINVER >= 0x0500)
/*阿拉伯以及西伯来版本的98/ME,2000/XP创建一个水平起点在右边的窗口.
越往左边水平坐标值变大.Right to left mirroring*/
#define WS_EX_LAYOUTRTL         0x00400000L
#endif /* WINVER >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)
/*将一个窗体的所有子窗口使用双缓冲按照从低到高方式绘制出来,参阅remark项.
如果这个视窗已经使用经典样式中的下列值CS_OWNDC , CS_CLASSDC,WS_EX_CONTEXTHELP.此参数将不能使用.
这个样式的视窗在标题栏上有一个问号,当拥护点击着个问号,鼠标变成一个问号,
如果用户然后点击一个子窗口,子窗就会收到一条WM_HELP消息.
子窗口将把这个消息传递给他的父进程,这个父进程将用HELP_WM_HELP命令调用WinHelp函数.
这个帮助程序常常弹出一个典型的包含其子窗口的帮助的窗口
本参数不能和WS_MAXIMIZEBOX ,WS_MINIMIZEBOX一起使用*/
#define WS_EX_COMPOSITED        0x02000000L
#endif /* _WIN32_WINNT >= 0x0501 */
#if(_WIN32_WINNT >= 0x0500)
/*一个使用此参数创建的顶级窗口不会变成前台窗口,
当用户点击他时.系统不会将此窗口放到前台,当用户最小化或者关闭这个前台窗口.
要激活这样的窗口,使用SetActiveWindow或者SetForegroundWindow函数
此类型的窗口默认不会显示在任务栏上.要强行将这样的窗口显示到任务栏上,使用WS_EX_APPWINDOW参数.*/
#define WS_EX_NOACTIVATE        0x08000000L
#endif /* _WIN32_WINNT >= 0x0500 */

#endif


