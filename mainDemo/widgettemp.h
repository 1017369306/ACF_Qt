#ifndef WIDGETTEMP_H
#define WIDGETTEMP_H

#include <QWidget>

namespace Ui {
class widgetTemp;
}

class widgetTemp : public QWidget
{
    Q_OBJECT

public:
    explicit widgetTemp(QWidget *parent = nullptr);
    ~widgetTemp();

private:
    Ui::widgetTemp *ui;
};

#endif // WIDGETTEMP_H
