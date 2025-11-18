#include <QApplication>
#include <QTimer>
#include "MyWidget.h"
#include "WidgetView.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 注册元类型（必须！否则信号槽无法传递自定义结构体）
    qRegisterMetaType<FunctionData>();
    qRegisterMetaType<SetFunctionMetaData>();
    qRegisterMetaType<WidgetView::WidgetViewMap>();
    qRegisterMetaType<WidgetView::FunctionType>();

    // 创建视图和模型
    MyWidgetView view;
    MyWidgetModel model;

    // 建立模型与视图的关联（核心步骤）
    model.SetView(&view);

    // 模拟模型主动更新数据（比如外部事件触发）
    QTimer::singleShot(2000, [&model]() {
        qDebug() << "\n===== 2秒后模型主动更新数据 =====";
        WidgetView::WidgetViewMap updateData;
        // 更新亮度值到90
        updateData.insert(
            WidgetView::EM_SETTING_FUNCTION_BRIGHTNESS,
            std::make_shared<FunctionData>(
                WidgetView::EM_SETTING_FUNCTION_BRIGHTNESS,
                false,
                90,
                true
                )
            );
        // 添加新数据：音量设置
        updateData.insert(
            WidgetView::EM_SETTING_FUNCTION_VOLUME,
            std::make_shared<FunctionData>(
                WidgetView::EM_SETTING_FUNCTION_VOLUME,
                false,
                70,
                true
                )
            );
        model.SetViewData(updateData);
    });

    // 显示视图
    view.resize(400, 300);
    view.show();

    return app.exec();
}
