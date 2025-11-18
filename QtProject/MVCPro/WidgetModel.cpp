#include "WidgetModel.h"

WidgetModel::WidgetModel(QObject *parent)
    : QObject(parent)
    , m_view(nullptr)
{
}

WidgetModel::~WidgetModel() {}

void WidgetModel::SetView(WidgetView *view)
{
    if (m_view && m_view != view) {
        disconnect(m_view, &WidgetView::FunctionDataChanged, this, &WidgetModel::FunctionDataChangedSlot);
        disconnect(this, &WidgetModel::SetFunctionDataSignal, m_view, &WidgetView::SetFunctionDataSlot);
    }

    m_view = view;

    connect(m_view, &WidgetView::FunctionDataChanged, this, &WidgetModel::FunctionDataChangedSlot);
    connect(this, &WidgetModel::SetFunctionDataSignal, m_view, &WidgetView::SetFunctionDataSlot);

    //初始化界面
    _initView();
}

WidgetView *WidgetModel::GetView() const
{
    return m_view;
}

void WidgetModel::SetViewData(const WidgetView::WidgetViewMap &map)
{
    QList<QVariant> list;
    SetFunctionMetaData metaData;

    for (auto iter = map.begin(); iter != map.end(); iter++) {
        metaData.type = iter.key();
        metaData.data = iter.value();
        list.append(QVariant::fromValue(metaData));
    }

    //note:使用信号跨线程传递时，携带的参数必须是元数据；否则槽函数不会触发
    emit SetFunctionDataSignal(list);
}

void WidgetModel::FunctionDataChangedSlot(FunctionData data)
{
    //to do nothing
}
