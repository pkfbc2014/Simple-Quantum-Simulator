#ifndef EXAMPLE3_H
#define EXAMPLE3_H

#include <QWidget>

namespace Ui {
class example3;
}

class example3 : public QWidget
{
    Q_OBJECT

public:
    explicit example3(QWidget *parent = nullptr);
    ~example3();

private:
    Ui::example3 *ui;
};

#endif // EXAMPLE3_H
