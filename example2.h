#ifndef EXAMPLE2_H
#define EXAMPLE2_H

#include <QWidget>

namespace Ui {
class example2;
}

class example2 : public QWidget
{
    Q_OBJECT

public:
    explicit example2(QWidget *parent = nullptr);
    ~example2();

private:
    Ui::example2 *ui;
};

#endif // EXAMPLE2_H
