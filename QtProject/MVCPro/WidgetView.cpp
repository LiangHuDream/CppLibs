#include <QDebug>
#include "WidgetView.h"

WidgetView::WidgetView(QWidget *parent)
    : QWidget(parent)
{
}

WidgetView::~WidgetView() {}

QMap<WidgetView::FunctionType, std::shared_ptr<FunctionData>> WidgetView::GetFunctionData()
{
    return QMap<WidgetView::FunctionType, std::shared_ptr<FunctionData>>();
}

FunctionData WidgetView::GetSettingFunction(FunctionType type)
{
    return FunctionData();
}

void WidgetView::SetFunctionData(WidgetViewMap data) {}

void WidgetView::SetFunctionDataSlot(QVariant data)
{
    QList<QVariant> list = data.toList();
    WidgetView::WidgetViewMap map;
    SetFunctionMetaData metaData;
    int size = list.size();

    for (int i = 0; i < size; i++) {
        metaData = list[i].value<SetFunctionMetaData>();
        map.insert(metaData.type, metaData.data);
    }

    SetFunctionData(map);
}
