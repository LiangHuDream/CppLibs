#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "WidgetView.h"
#include "WidgetModel.h"
#include <QDebug>

// Specific view class (inherits from WidgetView)
class MyWidgetView : public WidgetView
{
    Q_OBJECT
public:
    explicit MyWidgetView(QWidget *parent = nullptr) : WidgetView(parent) {
        qDebug() << "MyWidgetView initialized";
    }

    // Override method to get function data
    QMap<FunctionType, std::shared_ptr<FunctionData>> GetFunctionData() override {
        return m_funcMap;
    }

    // Override method to set function data
    void SetFunctionData(WidgetViewMap data) override {
        m_funcMap = data;
        qDebug() << "View received data update, total" << data.size() << "records";

        // Traverse and print updated data
        for (auto iter = data.begin(); iter != data.end(); ++iter) {
            auto funcData = iter.value();
            qDebug() << "Type:" << iter.key()
                     << " Value:" << funcData->value
                     << " Checked status:" << funcData->checked;
        }

        // Emit data change signal (simulate data change caused by user operation)
        if (!data.isEmpty()) {
            emit FunctionDataChanged(*data.first());
        }
    }

private:
    WidgetViewMap m_funcMap; // Store view data
};

// Specific model class (inherits from WidgetModel)
class MyWidgetModel : public WidgetModel
{
    Q_OBJECT
public:
    explicit MyWidgetModel(QObject *parent = nullptr) : WidgetModel(parent) {
        qDebug() << "MyWidgetModel initialized";
    }

protected:
    // Override method to initialize view
    void _initView() override {
        qDebug() << "Model initializing view data";

        // Build initial data (brightness, contrast, etc. settings)
        WidgetView::WidgetViewMap initData;

        // Add brightness data (Type 1: Brightness, Value 80, Checked true)
        initData.insert(
            WidgetView::EM_SETTING_FUNCTION_BRIGHTNESS,
            std::make_shared<FunctionData>(
                WidgetView::EM_SETTING_FUNCTION_BRIGHTNESS,
                false,  // Not hidden
                80,     // Brightness value
                true    // Checked
                )
            );

        // Add contrast data (Type 2: Contrast, Value 50, Checked false)
        initData.insert(
            WidgetView::EM_SETTING_FUNCTION_CONTRANST,
            std::make_shared<FunctionData>(
                WidgetView::EM_SETTING_FUNCTION_CONTRANST,
                false,  // Not hidden
                50,     // Contrast value
                false   // Not checked
                )
            );

        // Send initial data to view
        SetViewData(initData);
    }

    // Override slot function to handle view data changes
    void FunctionDataChangedSlot(FunctionData data) override {
        qDebug() << "\nModel received view data change:";
        qDebug() << "Change type:" << data.id.value<WidgetView::FunctionType>();
        qDebug() << "Change value:" << data.value;
    }
};

#endif // MYWIDGET_H
