#ifndef _WIDGET_MODEL_H_
#define _WIDGET_MODEL_H_

#include <QObject>
#include "WidgetView.h"

class WidgetModel : public QObject
{
    Q_OBJECT

public:
    explicit WidgetModel(QObject *parent = nullptr);
    virtual ~WidgetModel();

    virtual void SetView(WidgetView *view);
    virtual WidgetView *GetView() const;

public:
    virtual void _initView() = 0;
    virtual void SetViewData(const WidgetView::WidgetViewMap &map);

private slots:
    virtual void FunctionDataChangedSlot(FunctionData data);

signals:
    void SetFunctionDataSignal(QVariant data);

private:
    WidgetView *m_view;
};

#endif  // ! _WIDGET_MODEL_H_
