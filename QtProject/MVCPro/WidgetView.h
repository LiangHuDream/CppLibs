#ifndef _WIDGET_VIEW_H_
#define _WIDGET_VIEW_H_

#include <QWidget>
#include <QMap>
#include <QVariant>
#include <QMetaType>
#include <memory>
#include "common.h"

#define DECLARE_OBJ(obj, type, ...)                                                                                    \
type *obj = new type(__VA_ARGS__);                                                                                 \
    obj->setObjectName(#obj);

#define FIND_CHILD(obj, parent, type) type *obj = parent->findChild<type *>(#obj);

#define CHANGE_TO_OBJ(obj) (#obj)

#define GAMMA_COUNT (21)  //1.0 ~ 3.0
#define GAMMA_MAX   (256)
#define GAMMA_START (1.0)
#define GAMMA_STEP  (0.1)

struct FunctionData;
struct TipsData;

class WidgetView : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QMap<FunctionType, std::shared_ptr<FunctionData>> FunctionValue READ GetFunctionData WRITE
                   SetFunctionData NOTIFY FunctionDataChanged)

public:
    enum DisplayMode
    {
        IDLE,          //未定义
        NormalMode,    //正常模式
        HideMode,      //隐藏模式
        RecordMode,    //录制模式
        PlaybackMode,  //回放模式
        FullAnalysisChartMode,  //全分析图模式 //note:全分析图修改为普通分析功能，不再作为一种界面状态
        AllLockScreenMode,       //全锁屏模式:隐藏所有控件
        HalfLockScreenMode,      //半锁屏模式:仅保留解锁控件
        RecordLockScreenMode,    //录制锁屏模式：仅保留解锁控件 + 录制时长显示控件
        PlaybackLockScreenMode,  //回放锁屏模式：不修改任何控件的显示状态，仅不与用户交互 --> 此模式下只有使用快捷键才能退出
        PlaybackHideMode,        //回放隐藏模式
    };
    Q_ENUM(DisplayMode)

    /*频点设置*/
    enum ChannelType
    {
        EM_CHANNEL_AUTO,

        EM_CHANNEL_1,
        EM_CHANNEL_2,
        EM_CHANNEL_3,
        EM_CHANNEL_4,
        EM_CHANNEL_5,
        EM_CHANNEL_6,
        EM_CHANNEL_7,
        EM_CHANNEL_8,
        EM_CHANNEL_9,
        EM_CHANNEL_10,
        EM_CHANNEL_11,
        EM_CHANNEL_12,
        EM_CHANNEL_13,

        EM_CHANNEL_SCAN,
    };
    Q_ENUM(ChannelType)

    /*异常提示码*/
    enum TipsErrNo
    {
        //外设异常提示
        EM_TIPS_NO_SD_CARD,                  //未接入SD卡
        EM_TIPS_INSUFFICIENT_SD_CARD_SPACE,  //SD卡空间不足
        EM_TIPS_NO_U_DISK,                   //未接入U盘
        EM_NO_VIDEO_FILE,                    //未找到视频文件
        EM_NO_VIDEO_INPUT,                   //无视频输入

        //内部逻辑异常提示 --> 对内有效
        EM_PLAYBACK_STATE_ERR,  //回放状态异常
        EM_VIDEO_FILE_TOO_BIG,  //视频文件太大，不支持播放

        //操作失败提示
        EM_TIPS_SCREEN_SHOT_FAIL,      //截图失败
        EM_TIPS_PLAYBACK_FAIL,         //回放失败
        EM_TIPS_LUT_IMPORT_FAILED,     //LUT导入失败
        EM_TIPS_SDCARD_FORMAT_FAILED,  //格式化SD卡失败
        EM_TIPS_SET_FAILED,            //设置失败

        //操作成功提示
        EM_TIPS_LUT_IMPORT_SUCCESS,     //LUT导入成功
        EM_TIPS_SDCARD_FORMAT_SUCCESS,  //格式化SD卡成功
        EM_TIPS_SET_SUCCESS,            //设置成功

        //界面提示
        EM_TIPS_SCREEN_LOCK,       //屏幕已锁定
        EM_TIPS_SCREEN_UNLOCK,     //屏幕已解锁
        EM_TIPS_OPEN_DEBUG_MODE,   //开启调试模式
        EM_TIPS_CLOSE_DEBUG_MODE,  //关闭调试模式
    };
    Q_ENUM(TipsErrNo)

    /*正常提示码*/
    enum TipsNormalNo
    {
        EM_TIPS_RESET_FACTORY,       //恢复出厂设置
        EM_TIPS_FORMAT_SDCARD,       //格式化SD卡
        EM_TIPS_SET_STATICFRAME,     //设置静帧图片
        EM_TIPS_RESET_DISPLAY,       //重置显示设置参数
        EM_TIPS_FIND_HIGHT_VERSION,  //检测到新版本
        EM_TIPS_UPGRADE_FAILED,      //升级失败
        EM_TIPS_CLICKED_OTA,         //点击版本

        EM_TIPS_FOUND_VERSION,       //寻找到新版本

        EM_TIPS_WAITING_WPS,         //wps配对  转圈有取消按键
        EM_TIPS_WAITING_NOWPS,       //非wps配对  转圈没有取消按键

        EM_TIPS_RESET_66121,         //复位66121
        EM_TIPS_CHANGE_DEVTYPE,      //切换TX/RX
        EM_TIPS_CHANGE_CONTYPE,      //切换一发多/二发一
        EM_TIPS_FOLLOW_FOCUS_MODE,   //跟焦模式提示
    };
    Q_ENUM(TipsNormalNo)

    enum FunctionType
    {
        EM_FUNCTION_DILE,  //未定义

        //多屏交互
        EM_DISPLAY_FUNCTION_MULTI_SCREEN_INTERACTION,  //多屏交互模式更改
        EM_DISPLAY_FUNCTION_SCREEN_CHANGED,            //当前选择屏幕更改

        //主界面
        EM_DISPLAY_MODE,      //主界面的显示模式
        EM_DISPLAY_LOGO,      //跳动logo
        EM_DISPLAY_FUNCTION,  //分析功能点击，显示二级菜单入口
        EM_DISPLAY_ENABLE,    //主界面使能

        //顶部状态栏
        EM_DISPLAY_FUNCTION_DEVICE_TYPE,        //设备类型
        EM_DISPLAY_FUNCTION_PAIR_LOCK,          //锁定分组
        EM_DISPLAY_FUNCTION_CHANNEL,            //频点
        EM_DISPLAY_FUNCTION_GROUP,              //分组
        EM_DISPLAY_FUNCTION_CONNECT,            //TX连接数量/RX信号格
        EM_DISPLAY_FUNCTION_RX_CONNECT_NUM,     //RX连接数量(信号格图标,连接数量文本)
        EM_DISPLAY_FUNCTION_SCREEN_MODE,        //情景模式
        EM_DISPLAY_FUNCTION_BROADCAST,          //广播模式
        EM_DISPLAY_FUNCTION_FOLLOW_FOCUS_MODE,  //跟焦模式
        EM_DISPLAY_FUNCTION_MULTI_CONNECT,      //两发一收模式
        EM_DISPLAY_FUNCTION_INPUT_SOURCE,       //输入源
        EM_DISPLAY_FUNCTION_RESOLUTION,         //分辨率
        EM_DISPLAY_FUNCTION_LUT,                //lut
        EM_DISPLAY_FUNCTION_SD,                 //SD卡
        EM_DISPLAY_FUNCTION_U_DISK,             //USB设备
        EM_DISPLAY_FUNCTION_HEADSET,            //耳机
        EM_DISPLAY_FUNCTION_BATTERY,            //电池图标
        EM_DISPLAY_FUNCTION_BATTERY_TEXT,       //电池文本
        EM_DISPLAY_FUNCTION_ENLARGE,            //放大倍数

        //底部页面切换按钮
        EM_DISPLAY_FUNCTION_ANALYSIS,  //分析功能页
        EM_DISPLAY_FUNCTION_TEMPLATE,  //模板切换页
        EM_DISPLAY_FUNCTION_CAMERA,    //相机控制切换页
        EM_DISPLAY_FUNCTION_REPLAY,    //回放切换页
        EM_DISPLAY_FUNCTION_RECORD,    //录制切换页 todo:待定

        //右侧控制功能
        EM_DISPLAY_FUNCTION_LOCK,         //锁屏
        EM_DISPLAY_FUNCTION_SCREEN_SHOT,  //截图结果显示

        //底部分析功能
        EM_ANALYSIS_FUNCTION_PAGE,         //当前分析功能的页面ID
        EM_ANALYSIS_FUNCTION_WAVE,         //波形图 --> 此处仅包含控制信息
        EM_ANALYSIS_FUNCTION_VECTOR,       //矢量图 --> 此处仅包含控制信息
        EM_ANALYSIS_FUNCTION_HISOTGRAM,    //直方图 --> 此处仅包含控制信息
        EM_ANALYSIS_FUNCTION_ZEBRA,        //斑马纹
        EM_ANALYSIS_FUNCTION_PSEUDO,       //伪彩色
        EM_ANALYSIS_FUNCTION_FOCUS,        //辅助对焦
        EM_ANALYSIS_FUNCTION_LUT,          //LUT
        EM_ANALYSIS_FUNCTION_CENTER_MARK,  //中心标记
        EM_ANALYSIS_FUNCTION_SHADE_MARK,   //遮幅标记
        EM_ANALYSIS_FUNCTION_SAFE,         //安全框
        EM_ANALYSIS_FUNCTION_SUDOKU,       //九宫格
        EM_ANALYSIS_FUNCTION_DEFORMATION,  //变形
        EM_ANALYSIS_FUNCTION_ROTATION,     //旋转
        EM_ANALYSIS_FUNCTION_MIRROR,       //镜像
        EM_ANALYSIS_FUNCTION_ONION,        //图像叠加(洋葱皮)
        EM_ANALYSIS_FUNCTION_FULL_CHART,   //全分析图
        EM_ANALYSIS_FUNCTION_ENLARGE,      //放大

        //分析功能二级菜单
        EM_ANALYSIS_FUNCTION_RESET,           //分析功能重置
        EM_ANALYSIS_FUNCTION_LUT_SELECT,      //用户lut选择
        EM_ANALYSIS_FUNCTION_LUT_IMPORT,      //用户lut导入
        EM_ANALYSIS_FUNCTION_PICTURE_IMPORT,  //叠加图片选择

        EM_SETTING_FUNCTION_EXIT,             //设置界面回退

        // 设置界面
        EM_SETTING_FUNCTION_PAGE,           //当前页面ID
        EM_SETTING_FUNCTION_PAGE_DISPLAY,   //显示设置界面
        EM_SETTING_FUNCTION_PAGE_FUNCTION,  //功能设置界面
        EM_SETTING_FUNCTION_PAGE_SYSTEM,    //系统设置界面
        EM_SETTING_FUNCTION_PAGE_DEVICE,    //设备信息界面
        EM_SETTING_FUNCTION_PAGE_FILE,      //文件管理界面
        EM_SETTING_FUNCTION_PAGE_DEBUG,     //调试界面

        //显示设置
        EM_SETTING_FUNCTION_RESETDISPLAY,         // 重置显示设置
        EM_SETTING_FUNCTION_BRIGHTNESS,           // 亮度
        EM_SETTING_FUNCTION_SATURABILITY,         // 饱和度设置
        EM_SETTING_FUNCTION_CONTRANST,            // 对比度设置
        EM_SETTING_FUNCTION_ACUTANCE,             // 锐度设置
        EM_SETTING_FUNCTION_EXPOSURE,             // 曝光设置
        EM_SETTING_FUNCTION_COLORTEMPERATURE,     // 色温
        EM_SETTING_FUNCTION_USERLUT,              // 用户LUT
        EM_SETTING_FUNCTION_IMPORTLUT,            // 导入用户/工厂校准LUT
        EM_SETTING_FUNCTION_DISPLAY_CORRECT_LUT,  // 显示校准LUT
        EM_SETTING_FUNCTION_GAMMA,                // gamma设置
        EM_SETTING_FUNCTION_R_PROPORTION,         // R占比
        EM_SETTING_FUNCTION_R_GAIN,               // R增益
        EM_SETTING_FUNCTION_G_PROPORTION,         // G占比
        EM_SETTING_FUNCTION_G_GAIN,               // G增益
        EM_SETTING_FUNCTION_B_PROPORTION,         // B占比
        EM_SETTING_FUNCTION_B_GAIN,               // B增益

        //功能设置
        EM_SETTING_FUNCTION_ANNOTATION,           // 标记
        EM_SETTING_FUNCTION_STATIC_FRAME,         // 静帧
        EM_SETTING_FUNCTION_VOLUME_COLUMN,        // 音量柱
        EM_SETTING_FUNCTION_FAN_MODE,             // 风扇模式
        EM_SETTING_FUNCTION_SHORTCUT_KEY1,        // 快捷键F1
        EM_SETTING_FUNCTION_SHORTCUT_KEY2,        // 快捷键F2
        EM_SETTING_FUNCTION_VOLUME,               // 音量
        EM_SETTING_FUNCTION_TRIGGER,              // trigger
        EM_SETTING_FUNCTION_CAMERA_MANUFACTURER,  // 相机厂商
        EM_SETTING_FUNCTION_CAMERA_TYPE,          // 相机型号
        EM_SETTING_FUNCTION_VIDEO_PREFIX,         // 录制视频前缀

        //系统设置
        EM_SETTING_FUNCTION_WIRELESS,         // 无线功能
        EM_SETTING_FUNCTION_DEVICETYPE,       // 更改设备模式
        EM_SETTING_FUNCTION_CONNECTION_MODE,  //连接模式 单点和多点
        EM_SETTING_FUNCTION_BOARDCASTMODE,    //广播模式
        EM_SETTING_FUNCTION_SCEENMODE,        //情景模式
        EM_SETTING_FOLLOW_FOCUS_MODE,         //跟焦模式
        EM_SETTING_FUNCTION_LANGAUGE,         //语言设置
        EM_SETTING_FUNCTION_CHANNEL_LIST,     //频点列表
        EM_SETTING_FUNCTION_CHANNEL,          //无线频点
        EM_SETTING_FUNCTION_SCAN,             //扫频
        EM_SETTING_FUNCTION_PAIRGROUP,        //分组切换
        EM_SETTING_FUNCTION_INPUTSIGNAL,      //切换输入信号 --> 仅TX
        EM_SETTING_FUNCTION_OUTPUT_SIGNAL,    //切换输出信号 --> 仅RX
        EM_SETTING_FUNCTION_PAIRLOCK,         //锁定配对

        //设备信息
        EM_SETTING_FUNCTION_WIFISSID,       //ssid
        EM_SETTING_FUNCTION_WIFIPASSWORD,   //密码
        EM_SETTING_FUNCTION_SN,             //sn
        EM_SETTING_FUNCTION_VERSIONINFO,    //版本信息
        EM_SETTING_FUNCTION_FOUNDVERSION,   //更新版本
        EM_SETTING_FUNCTION_SDSTORAGE,      //显示SD卡容量
        EM_SETTING_FUNCTION_SDSTORAGEBAR,   //SD卡容量进度条
        EM_SETTING_FUNCTION_FORMAT_SDCARD,  //格式化SD卡
        EM_SETTING_FUNCTION_FACTORY,        //恢复出厂设置

        //文件管理
        EM_FILECONTROL_SET_STATICFRAME,  //设置静帧图片
        EM_FILECONTROL_CLICKED_OTA,      //点击版本文件
        EM_FILECONTROL_VIDEO,            //点击视频文件

        //手势交互
        EM_ANALYSIS_FUNCTION_VECTOR_POSITION,           //矢量图位置 --> 此处仅包含位置信息
        EM_ANALYSIS_FUNCTION_WAVE_POSITION,             //波形图位置 --> 此处仅包含位置信息
        EM_ANALYSIS_FUNCTION_HISTOGRAM_POSITION,        //直方图位置 --> 此处仅包含位置信息
        EM_ANALYSIS_FUNCTION_FULL_CHART_WAVE_POSITION,  //全分析图下波形图位置 --> 此处仅包含位置信息
        EM_ANALYSIS_FUNCTION_FULL_CHART_VECTOR_POSITION,  //全分析图下矢量图位置 --> 此处仅包含位置信息
        EM_ANALYSIS_FUNCTION_FULL_CHART_HISTOGRAM_POSITION,  //全分析图下直方图位置 --> 此处仅包含位置信息
        EM_ANALYSIS_FUNCTION_PSEUDO_TABLE,                   //伪彩色映射表
        EM_ANALYSIS_FUNCTION_VOLUME_COLUMN_POSITION,         //音量柱位置 --> 此处仅包含位置信息
        EM_SHARED_ANNOTATION,                                //共享标注

        //录制控制
        EM_RECORD_TIMESTAMP,    //录制时间戳
        EM_RECORD_FAVORITE,     //录制收藏
        EM_RECORD_SCREEN_SHOT,  //屏幕截图
        EM_STOP_RX_RECORD_PIC,  // 接收端视频流暂停图示

        //回放控制
        EM_QUICK_PLAYBACK_VIDEO_LIST,     //快捷回放列表
        EM_QUICK_PLAYBACK_CURRENT_VIDEO,  //当前回放的视频
        EM_QUICK_PLAYBACK_PLAY,           //回放暂停/播放
        EM_QUICK_PLAYBACK_TIMESTAMP,      //回放时间戳
        EM_QUICK_PLAYBACK_SLIDER,         //回放滑条

        //升级
        EM_UPGRADE_FUNCTION_SD,      //SD卡是否有高版本
        EM_UPGRADE_FUNCTION_U_DISK,  //U盘是否有高版本
        EM_UPGRADE_FUNCTION_START,   //升级开始
        EM_UPGRADE_FUNCTION_FAILED,  //升级失败
        EM_UPGRADE_FUNCTION_FINISH,  //升级完成

        //提示
        EM_EXCEPTION_TIPS,  //异常提示  有图标、无选项
        EM_NORMAL_TIPS,     //正常提示  无图标、有选项
        EM_WAITING_TIPS,    //等待提示  无图标、无选项

        //DEBUG
        EM_DEBUG_MODE,         //调试模式
        EM_DEBUG_FACTORY_LUT,  //出厂校准LUT
        EM_DEBUG_IMPORT_LUT,   //导入校准LUT
        EM_DEBUG_READ_LUT,     //读取校准LUT
        EM_DEBUG_FAN_SPEED,    // 风扇占空比
        EM_DEBUG_VIDEO_RATE,   // 视频码率
        EM_DEBUG_FRC_MODE,     // FRC开关
        EM_DEBUG_RESET_66121,  // 复位66121
        EM_DEBUG_SHOW_COLOR,   //显示色彩空间类型

        //关机界面
        EM_SHUTDOWN_CONFIRM,  //关机确认
    };
    Q_ENUM(FunctionType)

    using WidgetViewMap = QMap<WidgetView::FunctionType, std::shared_ptr<FunctionData>>;

    explicit WidgetView(QWidget *parent = nullptr);
    virtual ~WidgetView();

    virtual QMap<WidgetView::FunctionType, std::shared_ptr<FunctionData>> GetFunctionData();
    virtual FunctionData GetSettingFunction(WidgetView::FunctionType type);
    virtual void SetFunctionData(WidgetViewMap data);

public slots:
    void SetFunctionDataSlot(QVariant data);

signals:
    void FunctionDataChanged(FunctionData data);
};

struct FunctionData
{
    QVariant id;
    bool isReset;    //是否重置
    bool isHidden;   //是否隐藏
    bool checked;    //是否选中
    QVariant value;  //图标或文本等

    FunctionData()
        : FunctionData(WidgetView::EM_FUNCTION_DILE, false, QVariant(), false)
    {
    }

    FunctionData(WidgetView::FunctionType type, bool isHidden, QVariant value, bool checked = false, bool reset = false)
        : id(type)
        , isHidden(isHidden)
        , value(value)
        , checked(checked)
        , isReset(reset)
    {
    }

    FunctionData(const FunctionData &other)
        : FunctionData(other.id.value<WidgetView::FunctionType>(),
                       other.isHidden,
                       other.value,
                       other.checked,
                       other.isReset)
    {
    }

    FunctionData &operator=(const FunctionData &other)
    {
        this->id = other.id;
        this->isHidden = other.isHidden;
        this->value = other.value;
        this->checked = other.checked;
        this->isReset = other.isReset;
        return *this;
    }
};
Q_DECLARE_METATYPE(FunctionData);

struct SetFunctionMetaData
{
    WidgetView::FunctionType type;
    std::shared_ptr<FunctionData> data;
};
Q_DECLARE_METATYPE(SetFunctionMetaData);

struct TipsData
{
    using Callback = std::function<void(QVariant)>;

    QVariant id;             //TipsErrNo or TipsNormalNo
    QVariant data;           //需要传输文本等
    Callback confirmHandle;  //确认动作处理
    Callback cancelHandle;   //取消动作处理
    QWidget *parent;

    TipsData()
        : TipsData(QVariant(), QVariant())
    {
    }

    TipsData(QVariant id,
             QVariant data,
             Callback confirmCallback = nullptr,
             Callback cancelCallback = nullptr,
             QWidget *parent = nullptr)
        : id(id)
        , data(data)
        , confirmHandle(confirmCallback)
        , cancelHandle(cancelCallback)
        , parent(parent)
    {
    }
    TipsData &operator=(const TipsData &other)
    {
        this->id = other.id;
        this->data = other.data;
        this->cancelHandle = other.cancelHandle;
        this->confirmHandle = other.confirmHandle;
        this->parent = other.parent;
        return *this;
    }
};
Q_DECLARE_METATYPE(TipsData);
Q_DECLARE_METATYPE(WidgetView::WidgetViewMap);

#endif  // !_WIDGET_VIEW_H_
